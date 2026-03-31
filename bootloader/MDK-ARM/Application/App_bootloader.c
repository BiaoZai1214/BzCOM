/**
 * @file App_bootloader.c
 * @brief Bootloader 启动流程状态机
 * @note 支持双区备份 OTA 和串口 IAP 两种升级模式
 */
#include "app_bootloader.h"
#include "iap.h"
#include "ota.h"

/*============================================================*/
/* 常量定义                                                   */
/*============================================================*/
#define KEY_DEBOUNCE_MS     50      /* 按键消抖时间 */
#define KEY_WAIT_MS         3000    /* 按键等待超时 */
#define FLASH_COPY_SIZE     FLASH_APP_SIZE /* 固件复制大小 */

/*============================================================*/
/* 全局变量                                                   */
/*============================================================*/
BootMode_t g_boot_mode = MODE_BOOT_NO_UPDATE;

uint32_t s_target_bank = ADDR_APP_B;   /* 目标分区（OTA/IAP 写入目标） */

static uint32_t s_active_bank  = ADDR_APP_A;   /* 当前激活分区 */

/* W25Q64 元数据 */
static uint8_t  s_meta[W25Q64_META_SIZE] = {0};
static uint32_t s_w25q64_size = 0;

/*============================================================*/
/* 内部函数                                                   */
/*============================================================*/

/* 固件有效性校验：栈顶地址必须在 SRAM 范围内 */
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

/* 读取 W25Q64 元数据 */
static void w25q64_read_meta(void)
{
    W25Q64_ReadData(W25Q64_META_ADDR, s_meta, W25Q64_META_SIZE);

    s_w25q64_size = s_meta[4] | ((uint32_t)s_meta[5] << 8) |
                    ((uint32_t)s_meta[6] << 16) | ((uint32_t)s_meta[7] << 24);

    /* 参数校验 */
    if (s_w25q64_size < APP_SIZE_MIN || s_w25q64_size > FLASH_APP_SIZE) {
        s_w25q64_size = 0;
        return;
    }

    /* 固件头校验 */
    uint8_t header[4];
    W25Q64_ReadData(W25Q64_FW_OFFSET, header, 4);
    uint32_t stack = header[0] | ((uint32_t)header[1] << 8) |
                     ((uint32_t)header[2] << 16) | ((uint32_t)header[3] << 24);

    if ((stack & 0xFFFF0000) != SRAM_STACK_BASE) {
        s_w25q64_size = 0;
    }
}

/* 保存 W25Q64 元数据 */
void w25q64_save_meta(uint32_t size)
{
    uint8_t meta[W25Q64_META_SIZE];
    meta[0] = (uint8_t)(W25Q64_FW_OFFSET >> 0);
    meta[1] = (uint8_t)(W25Q64_FW_OFFSET >> 8);
    meta[2] = (uint8_t)(W25Q64_FW_OFFSET >> 16);
    meta[3] = (uint8_t)(W25Q64_FW_OFFSET >> 24);
    meta[4] = (uint8_t)(size >> 0);
    meta[5] = (uint8_t)(size >> 8);
    meta[6] = (uint8_t)(size >> 16);
    meta[7] = (uint8_t)(size >> 24);
    W25Q64_WriteData(W25Q64_META_ADDR, meta, W25Q64_META_SIZE);
}

/* 擦除目标分区 */
void erase_target_bank(void)
{
    Boot_EraseFlash(s_target_bank, 8);
}

/* W25Q64 → Flash 分区 */
void copy_w25q64_to_flash(uint32_t size)
{
    HAL_FLASH_Unlock();
    uint8_t buff[256];
    uint32_t copied = 0;

    while (copied < size) {
        uint16_t len = (size - copied >= 256) ? 256 : (size - copied);
        W25Q64_ReadData(W25Q64_FW_OFFSET + copied, buff, len);

        for (uint16_t i = 0; i < len; i += 2) {
            uint16_t halfword = buff[i] | ((uint16_t)buff[i + 1] << 8);
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                              s_target_bank + copied + i, halfword);
        }
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
/* 串口 IAP 模式：串口接收 → 直接写目标分区                    */
/*============================================================*/
static void uart_iap_receive(void)
{
    erase_target_bank();
    IAP_Start(s_target_bank);

    while (!IAP_Process()) {}

    HAL_FLASH_Lock();

    if (uart_recv_done && is_valid_firmware(s_target_bank)) {
        switch_bank();
    }
}

/*============================================================*/
/* W25Q64 升级模式：已有固件在 W25Q64 中                      */
/*============================================================*/
static void w25q64_upgrade(void)
{
    w25q64_read_meta();
    if (s_w25q64_size == 0) return;

    erase_target_bank();
    copy_w25q64_to_flash(s_w25q64_size);

    if (is_valid_firmware(s_target_bank)) {
        switch_bank();
    }
}

/*============================================================*/
/* 启动流程                                                   */
/*============================================================*/

/* 检查 AT24C64，确定启动模式 */
void App_bootloader_check_update(void)
{
    uint8_t data[3];
    AT24C64_R_Bytes(ADDR_UPDATE_FLAG, data, 3);
    uint16_t key = ((uint16_t)data[1] << 8) | data[2];

    /* 密钥校验 */
    if (key != CHECK_KEY) {
        data[0] = (uint8_t)MODE_BOOT_NO_UPDATE;
        data[1] = (uint8_t)(CHECK_KEY >> 8);
        data[2] = (uint8_t)(CHECK_KEY & 0xFF);
        AT24C64_W_Bytes(ADDR_UPDATE_FLAG, data, 3);
        HAL_Delay(10);
        g_boot_mode = MODE_BOOT_NO_UPDATE;
    } else {
        g_boot_mode = (BootMode_t)data[0];
    }

    /* 根据模式设置分区 */
    switch (g_boot_mode) {
        case MODE_BOOT_UPDATE:
        case MODE_BOOT_UART_IAP:
        case MODE_BOOT_OTA:
            s_active_bank = Boot_GetActiveBank();
            s_target_bank = get_inactive_bank();
            break;

        case MODE_BOOT_NO_UPDATE:
        case MODE_BOOT_RESET:
            s_active_bank = Boot_GetActiveBank();
            break;

        default:
            g_boot_mode   = MODE_BOOT_NO_UPDATE;
            s_active_bank = Boot_GetActiveBank();
            break;
    }

    /* 活跃区固件无效时尝试备用区 */
    if (g_boot_mode == MODE_BOOT_NO_UPDATE && !is_valid_firmware(s_active_bank)) {
        uint32_t alt = get_inactive_bank();
        if (is_valid_firmware(alt)) {
            s_active_bank = alt;
        }
    }
}

/* 延时等待按键，决定进入哪种升级模式 */
void App_bootloader_check_default(void)
{
    uint32_t start_tick = HAL_GetTick();
    uint8_t key1_state = 0, key2_state = 0;
    uint32_t key1_tick = 0, key2_tick = 0;

    while (HAL_GetTick() - start_tick < KEY_WAIT_MS) {
        GPIO_PinState key1 = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
        GPIO_PinState key2 = HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);

        /* KEY1: OTA 模式 */
        if (key1 == GPIO_PIN_SET) {
            if (key1_state == 0) key1_tick = HAL_GetTick();
            key1_state = 1;
            if (HAL_GetTick() - key1_tick > KEY_DEBOUNCE_MS) {
                g_boot_mode   = MODE_BOOT_OTA;
                s_target_bank = get_inactive_bank();
                return;
            }
        } else {
            key1_state = 0;
        }

        /* KEY2: 串口 IAP 模式 */
        if (key2 == GPIO_PIN_SET) {
            if (key2_state == 0) key2_tick = HAL_GetTick();
            key2_state = 1;
            if (HAL_GetTick() - key2_tick > KEY_DEBOUNCE_MS) {
                g_boot_mode   = MODE_BOOT_UART_IAP;
                s_target_bank = get_inactive_bank();
                return;
            }
        } else {
            key2_state = 0;
        }
    }
}

/* 执行更新操作 */
void App_bootloader_update(void)
{
    switch (g_boot_mode) {
        case MODE_BOOT_UPDATE:
            w25q64_upgrade();
            break;
        case MODE_BOOT_UART_IAP:
            uart_iap_receive();
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

    Boot_CopyFlash(s_active_bank, ADDR_APP_RUN, FLASH_COPY_SIZE);
    Boot_JumpToApp(ADDR_APP_RUN);
}
