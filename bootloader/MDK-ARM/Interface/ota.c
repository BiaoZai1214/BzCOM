/**
 * @file ota.c
 * @brief OTA 升级逻辑
 * @note 支持协议帧模式: 串口接收 → W25Q64 → 目标 Flash 分区
 */
#include "ota.h"
#include "bootloader.h"
#include "App_bootloader.h"
#include "protocol.h"
#include <string.h>

/* 外部变量（定义在 usart.c） */
extern UART_HandleTypeDef huart1;
extern volatile uint8_t uart_recv_done;
extern volatile uint8_t uart_error_flag;
extern volatile uint32_t last_rec_time;

/* 外部变量（定义在 App_bootloader.c） */
extern uint32_t s_target_bank;

/*============================================================*/
/* 串口辅助                                                   */
/*============================================================*/
static void uart_send_str(const uint8_t *str)
{
    uint16_t len = 0;
    while (str[len]) len++;
    if (len > 0) HAL_UART_Transmit(&huart1, str, len, 100);
}

/*============================================================*/
/* OTA 接收处理 (协议帧模式)                                  */
/*============================================================*/
void OTA_Receive(void)
{
    Protocol_Handler_t proto;
    uint32_t total_size = 0;
    uint32_t write_addr = W25Q64_FW_OFFSET;
    uint8_t started = 0;
    uint8_t *buf = NULL;
    uint16_t len = 0;

    Protocol_Init(&proto);

    uart_send_str((const uint8_t *)"OTA_READY\r\n");

    /* 擦除 W25Q64 目标区域 */
    uint32_t sectors = (FLASH_APP_SIZE + 4096 - 1) / 4096;
    for (uint32_t i = 0; i < sectors; i++) {
        W25Q64_EraseSector(write_addr + i * 4096);
    }

    Boot_StartUartIap();

    while (1) {
        if (uart_error_flag) {
            uart_error_flag = 0;
        }

        /* 检查是否有数据 */
        if (uart_get_data(&buf, &len)) {
            last_rec_time = HAL_GetTick();

            if (!started) {
                /* 等待启动字符 */
                if (len >= 1 && buf[0] == IAP_START_CHAR) {
                    started = 1;
                    write_addr = W25Q64_FW_OFFSET;
                    total_size = 0;
                    uart_send_str((const uint8_t *)"ACK\r\n");
                }
                memset(buf, 0, len);
                continue;
            }

            /* 协议帧解析 */
            for (uint16_t i = 0; i < len; i++) {
                if (Protocol_ProcessByte(&proto, buf[i])) {
                    Protocol_Frame_t *frame = &proto.frame;

                    switch (frame->cmd) {
                        case CMD_UPDATE_START: {
                            if (frame->len >= 4) {
                                total_size = frame->data[0] |
                                            ((uint32_t)frame->data[1] << 8) |
                                            ((uint32_t)frame->data[2] << 16) |
                                            ((uint32_t)frame->data[3] << 24);
                                write_addr = W25Q64_FW_OFFSET;
                            }
                            Protocol_SendACK(frame->cmd);
                            break;
                        }

                        case CMD_UPDATE_DATA: {
                            if (frame->len > 4) {
                                uint32_t offset = frame->data[0] |
                                                 ((uint32_t)frame->data[1] << 8) |
                                                 ((uint32_t)frame->data[2] << 16) |
                                                 ((uint32_t)frame->data[3] << 24);
                                uint16_t data_len = frame->len - 4;
                                uint32_t addr = W25Q64_FW_OFFSET + offset;
                                W25Q64_WriteData(addr, &frame->data[4], data_len);
                            }
                            Protocol_SendACK(frame->cmd);
                            break;
                        }

                        case CMD_UPDATE_END: {
                            uart_recv_done = 1;
                            Protocol_SendACK(frame->cmd);
                            goto OTA_COMPLETE;
                        }

                        default:
                            Protocol_SendNAK(frame->cmd);
                            break;
                    }
                }
            }
            memset(buf, 0, len);
        }

        /* 超时检测 */
        if (started && (HAL_GetTick() - last_rec_time > IAP_IDLE_TIMEOUT)) {
            uart_recv_done = 1;
            break;
        }
        if (HAL_GetTick() - last_rec_time > IAP_TOTAL_TIMEOUT) {
            break;
        }
    }

OTA_COMPLETE:
    if (uart_recv_done && total_size > 0) {
        w25q64_save_meta(total_size);
        Boot_EraseFlash(s_target_bank, 8);
        copy_w25q64_to_flash(total_size);

        if (is_valid_firmware(s_target_bank)) {
            switch_bank();
            uart_send_str((const uint8_t *)"UPDATE_OK\r\n");
        } else {
            uart_send_str((const uint8_t *)"INVALID_FW\r\n");
        }
    } else {
        uart_send_str((const uint8_t *)"UPDATE_FAIL\r\n");
    }
}
