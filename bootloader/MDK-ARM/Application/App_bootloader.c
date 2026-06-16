/**
 * @file App_bootloader.c
 * @brief Bootloader 启动流程状态机
 */
#include "app_bootloader.h"
#include "ota.h"
#include "usart.h"



/*============================================================*/
/* 全局变量                                                   */
/*============================================================*/
BootMode_t g_boot_mode = MODE_BOOT_NO_UPDATE;
uint32_t s_target_bank = ADDR_APP_B;

static uint32_t s_active_bank = ADDR_APP_A;

/*============================================================*/
/* 静态函数                                                   */
/*============================================================*/

/* 固件有效性校验 */
uint8_t is_valid_firmware(uint32_t addr)
{
    uint32_t stack = *(volatile uint32_t *)addr;
    return (stack >= SRAM_STACK_BASE && stack <= SRAM_MAX);
}

/* 获取非活跃分区 */
static uint32_t get_inactive_bank(void)
{
    return (s_active_bank == ADDR_APP_A) ? ADDR_APP_B : ADDR_APP_A;
}

/* 等待按键，返回对应模式 */
static BootMode_t wait_for_key(void)
{
    uint32_t start = HAL_GetTick();

    while (HAL_GetTick() - start < KEY_WAIT_MS) {
        /* UART收到数据 → IAP裸数据直写 */
        if (uart_rx_buff.rx_len > 0) {
            return MODE_BOOT_UART_IAP;
        }

        /* KEY1 → OTA模式选择 */
        if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET) {
            uint32_t tick = HAL_GetTick();
            while (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET) {
                if (HAL_GetTick() - tick > KEY_DEBOUNCE_MS) {
                    return MODE_BOOT_OTA_SELECT;
                }
            }
        }

        /* KEY2 → UART IAP */
        if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET) {
            uint32_t tick = HAL_GetTick();
            while (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET) {
                if (HAL_GetTick() - tick > KEY_DEBOUNCE_MS) {
                    return MODE_BOOT_UART_IAP;
                }
            }
        }
    }
    return MODE_BOOT_NO_UPDATE;
}

/* 读取W25Q64固件大小 */
static uint32_t w25q64_read_size(void)
{
    uint8_t meta[W25Q64_META_SIZE];
    W25Q64_ReadData(W25Q64_META_ADDR, meta, W25Q64_META_SIZE);

    uint32_t size = meta[4] | ((uint32_t)meta[5] << 8) |
                    ((uint32_t)meta[6] << 16) | ((uint32_t)meta[7] << 24);

    if (size < APP_SIZE_MIN || size > FLASH_APP_SIZE) {
        return 0;
    }
    return size;
}

/* 保存W25Q64固件大小 */
void w25q64_save_meta(uint32_t size)
{
    uint8_t meta[W25Q64_META_SIZE] = {
        (uint8_t)(W25Q64_FW_OFFSET >> 0),  (uint8_t)(W25Q64_FW_OFFSET >> 8),
        (uint8_t)(W25Q64_FW_OFFSET >> 16), (uint8_t)(W25Q64_FW_OFFSET >> 24),
        (uint8_t)(size >> 0),              (uint8_t)(size >> 8),
        (uint8_t)(size >> 16),             (uint8_t)(size >> 24)
    };
    W25Q64_WriteData(W25Q64_META_ADDR, meta, W25Q64_META_SIZE);
}

/* W25Q64 → Flash */
void copy_w25q64_to_flash(uint32_t size)
{
    HAL_FLASH_Unlock();
    uint8_t buf[256];
    uint32_t copied = 0;

    while (copied < size) {
        uint16_t len = (size - copied >= 256) ? 256 : (size - copied);
        W25Q64_ReadData(W25Q64_FW_OFFSET + copied, buf, len);
        if (Flash_WriteBuffer(s_target_bank + copied, buf, len) != 0) break;
        copied += len;
    }
    HAL_FLASH_Lock();
}

/* 切换到目标分区 */
void switch_bank(void)
{
    Boot_SetActiveBank(s_target_bank);
    s_active_bank = s_target_bank;
}

/*============================================================*/
/* 升级模式处理                                                */
/*============================================================*/

/* UART IAP: 串口 → 直接写Flash */
void do_uart_iap(void)
{
    char dbg[32];
    if (Boot_EraseFlash(s_target_bank,
            (FLASH_APP_SIZE + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE) != 0) {
        return;
    }
    Boot_StartUartIap();

    /* Flash 擦除和 UART 初始化完成后才发送就绪信号，
     * 确保上位机发送数据时设备已准备好接收 */
    HAL_UART_Transmit(&huart1, (uint8_t *)"-> UART_IAP\r\n", 13, 100);

    uint32_t written = 0;
    uint32_t last_tick = HAL_GetTick();

    while (written < FLASH_APP_SIZE) {
        uint8_t *buf;
        uint16_t len;
        if (uart_get_data(&buf, &len)) {
            if (written + len <= FLASH_APP_SIZE) {
                HAL_FLASH_Unlock();
                if (Flash_WriteBuffer(s_target_bank + written, buf, len) != 0) {
                    HAL_FLASH_Lock();
                    break;
                }
                HAL_FLASH_Lock();
                written += len;
            }
            last_tick = HAL_GetTick();
        }
        if (written > 0 && HAL_GetTick() - last_tick > IAP_IDLE_TIMEOUT) break;
        if (HAL_GetTick() - last_tick > IAP_TOTAL_TIMEOUT) break;
    }
    HAL_FLASH_Lock();

    int n = sprintf(dbg, "DBG:%lu\r\n", written);
    HAL_UART_Transmit(&huart1, (uint8_t *)dbg, n, 100);

    if (written >= APP_SIZE_MIN && is_valid_firmware(s_target_bank)) {
        switch_bank();
        HAL_UART_Transmit(&huart1, (uint8_t *)"UPDATE_OK\r\n", 11, 100);
    } else {
        HAL_UART_Transmit(&huart1, (uint8_t *)"UPDATE_FAIL\r\n", 13, 100);
    }
}

/* W25Q64升级: W25Q64已有固件 → Flash */
static void do_w25q64_upgrade(void)
{
    uint32_t size = w25q64_read_size();
    if (size == 0) return;

    if (Boot_EraseFlash(s_target_bank,
            (FLASH_APP_SIZE + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE) != 0) return;
    copy_w25q64_to_flash(size);

    if (is_valid_firmware(s_target_bank)) {
        switch_bank();
    }
}

/*============================================================*/
/* 公共接口                                                    */
/*============================================================*/

/* 检查AT24C64，确定启动模式 */
void App_bootloader_check_update(void)
{
    uint8_t data[3];
    AT24C64_R_Bytes(ADDR_UPDATE_FLAG, data, 3);

    /* 密钥校验 */
    if (((uint16_t)data[1] << 8) | data[2] != CHECK_KEY) {
        data[0] = MODE_BOOT_NO_UPDATE;
        data[1] = CHECK_KEY >> 8;
        data[2] = CHECK_KEY & 0xFF;
        AT24C64_W_Bytes(ADDR_UPDATE_FLAG, data, 3);
        HAL_Delay(10);
        g_boot_mode = MODE_BOOT_NO_UPDATE;
    } else {
        g_boot_mode = (BootMode_t)data[0];
    }

    s_active_bank = Boot_GetActiveBank();

    switch (g_boot_mode) {
        case MODE_BOOT_UPDATE:
        case MODE_BOOT_UART_IAP:
        case MODE_BOOT_OTA:
        case MODE_BOOT_OTA_SELECT:
            s_target_bank = get_inactive_bank();
            break;
        default:
            break;
    }

    /* 活跃区无效则尝试备用区 */
    if (g_boot_mode == MODE_BOOT_NO_UPDATE && !is_valid_firmware(s_active_bank)) {
        uint32_t alt = get_inactive_bank();
        if (is_valid_firmware(alt)) {
            s_active_bank = alt;
        }
    }
}

/* 等待按键，决定进入哪种升级模式 */
void App_bootloader_check_default(void)
{
    BootMode_t mode = wait_for_key();
    if (mode != MODE_BOOT_NO_UPDATE) {
        g_boot_mode = mode;
        s_target_bank = get_inactive_bank();
    }
}

/* 执行更新操作 */
void App_bootloader_update(void)
{
    switch (g_boot_mode) {
        case MODE_BOOT_UPDATE:
            do_w25q64_upgrade();
            break;
        case MODE_BOOT_UART_IAP:
            do_uart_iap();
            break;
        case MODE_BOOT_OTA_SELECT:
            OTA_Select();
            if (g_boot_mode == MODE_BOOT_UART_IAP) do_uart_iap();
            break;
        case MODE_BOOT_OTA:
            OTA_Receive();
            break;
        default:
            break;
    }
}

/* 复制固件到运行区并跳转 */
void App_bootloader_copy_and_jump(void)
{
    if (g_boot_mode == MODE_BOOT_RESET) {
        Boot_JumpToApp(ADDR_FACTORY);
        return;
    }
    Boot_CopyFlash(s_active_bank, ADDR_APP_RUN, FLASH_APP_SIZE);
    Boot_JumpToApp(ADDR_APP_RUN);
}
