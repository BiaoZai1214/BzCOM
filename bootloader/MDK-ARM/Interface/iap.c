/**
 * @file iap.c
 * @brief UART IAP 升级逻辑
 * @note 直接写入目标 Flash 分区，不经过 W25Q64 中转
 */
#include "iap.h"
#include "bootloader.h"
#include "usart.h"
#include <string.h>

/* IAP 内部变量 */
static uint32_t s_target_addr  = 0;
static uint32_t s_flash_offset  = 0;
static uint32_t s_total_len    = 0;
static uint8_t  s_iap_done     = 0;

/* 外部变量 */
extern volatile uint8_t uart_recv_done;
extern volatile uint8_t uart_error_flag;
extern volatile uint32_t last_rec_time;

/*============================================================*/
/* 公共接口                                                   */
/*============================================================*/
void IAP_Start(uint32_t target_addr)
{
    s_target_addr  = target_addr;
    s_flash_offset = 0;
    s_total_len    = 0;
    s_iap_done     = 0;
    uart_recv_done = 0;

    Boot_StartUartIap();
}

uint8_t IAP_Process(void)
{
    if (s_iap_done) return 1;

    if (uart_error_flag) {
        uart_error_flag = 0;
    }

    uint8_t *buf;
    uint16_t len;
    if (uart_get_data(&buf, &len)) {
        last_rec_time = HAL_GetTick();

        /* 收到 '1' 启动接收 */
        if (s_flash_offset == 0 && len >= 1 && buf[0] == '1') {
            s_flash_offset = 1;  /* 标记已启动 */
        }
        /* 已启动，开始写 Flash */
        else if (s_flash_offset > 0) {
            if (s_total_len + len <= FLASH_APP_SIZE) {
                HAL_FLASH_Unlock();
                for (uint16_t i = 0; i < len; i += 2) {
                    uint16_t halfword = buf[i];
                    if (i + 1 < len) {
                        halfword |= (uint16_t)buf[i + 1] << 8;
                    }
                    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                                      s_target_addr + s_total_len + i, halfword);
                }
                HAL_FLASH_Lock();
                s_total_len += len;
            }
        }

        memset(buf, 0, len);
    }

    /* 空闲超时 → 传输结束 */
    if (s_flash_offset > 0 && (HAL_GetTick() - last_rec_time > IAP_IDLE_TIMEOUT)) {
        uart_recv_done = 1;
        s_iap_done     = 1;
    }

    /* 总超时保护 */
    if (HAL_GetTick() - last_rec_time > IAP_TOTAL_TIMEOUT) {
        s_iap_done = 1;
    }

    return s_iap_done;
}

uint8_t IAP_IsDone(void)
{
    return s_iap_done;
}

uint32_t IAP_GetReceivedLen(void)
{
    return s_total_len;
}
