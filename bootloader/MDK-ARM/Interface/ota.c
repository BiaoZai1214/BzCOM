/**
 * @file ota.c
 * @brief OTA 升级逻辑
 */
#include "ota.h"
#include "bootloader.h"
#include "App_bootloader.h"
#include "protocol.h"
#include <string.h>

/*============================================================*/
/* 外部变量（App_bootloader.c）                               */
/*============================================================*/
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
/* 协议帧处理                                                 */
/*============================================================*/
static uint32_t parse_u32(const uint8_t *data)
{
    return data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

/* 处理帧，返回1表示传输结束 */
static uint8_t handle_frame(Protocol_Frame_t *frame, uint32_t *total_size)
{
    switch (frame->cmd) {
        case CMD_UPDATE_START:
            *total_size = parse_u32(frame->data);
            Protocol_SendACK(frame->cmd);
            return 0;

        case CMD_UPDATE_DATA: {
            uint32_t offset = parse_u32(frame->data);
            uint16_t data_len = frame->len - 4;
            W25Q64_WriteData(W25Q64_FW_OFFSET + offset, &frame->data[4], data_len);
            Protocol_SendACK(frame->cmd);
            return 0;
        }

        case CMD_UPDATE_END:
            uart_recv_done = 1;
            Protocol_SendACK(frame->cmd);
            return 1;

        default:
            Protocol_SendNAK(frame->cmd);
            return 0;
    }
}

/* OTA接收主循环 */
static void ota_recv_loop(uint32_t *total_size)
{
    Protocol_Handler_t proto;
    uint8_t *buf = NULL;
    uint16_t len = 0;

    Protocol_Init(&proto);

    while (1) {
        if (uart_error_flag) uart_error_flag = 0;

        if (uart_get_data(&buf, &len)) {
            last_rec_time = HAL_GetTick();

            for (uint16_t i = 0; i < len; i++) {
                if (Protocol_ProcessByte(&proto, buf[i])) {
                    if (handle_frame(&proto.frame, total_size)) {
                        memset(buf, 0, len);
                        return;
                    }
                }
            }
            memset(buf, 0, len);
        }

        if (uart_recv_done) break;
        if (HAL_GetTick() - last_rec_time > IAP_TOTAL_TIMEOUT) break;
    }
}

/*============================================================*/
/* 公共接口                                                   */
/*============================================================*/

/* OTA接收（协议帧模式） */
void OTA_Receive(void)
{
    uint32_t total_size = 0;

    uart_send_str((const uint8_t *)"OTA_START\r\n");

    /* 擦除W25Q64 */
    for (uint32_t i = 0; i < (FLASH_APP_SIZE + 4095) / 4096; i++) {
        W25Q64_EraseSector(W25Q64_FW_OFFSET + i * 4096);
    }

    ota_recv_loop(&total_size);

    /* 校验并写入Flash */
    if (uart_recv_done && total_size > 0) {
        w25q64_save_meta(total_size);
        Boot_EraseFlash(s_target_bank, (FLASH_APP_SIZE + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE);
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

/* OTA模式选择 */
void OTA_Select(void)
{
    uint8_t *buf = NULL;
    uint16_t len = 0;
    uint32_t start = HAL_GetTick();

    Boot_StartUartIap();

    uart_send_str((const uint8_t *)"OTA_MODE:\r\n  1=UART\r\n  2=保留\r\n");

    while (HAL_GetTick() - start < 5000) {
        if (uart_get_data(&buf, &len)) {
            if (len >= 1) {
                if (buf[0] == '1') {
                    uart_send_str((const uint8_t *)"进入UART_OTA\r\n");
                    OTA_Receive();
                    return;
                }
                if (buf[0] == '2') {
                    uart_send_str((const uint8_t *)"模式2暂未实现\r\n");
                    return;
                }
            }
            memset(buf, 0, len);
        }
        HAL_Delay(10);
    }
    uart_send_str((const uint8_t *)"OTA_SELECT_TIMEOUT\r\n");
}
