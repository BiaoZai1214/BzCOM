/**
 * @file App_bootloader.c
 * @brief Bootloader 启动流程状态机
 * @note 支持双区备份 OTA 和串口 IAP 两种升级模式
 */
#include "app_bootloader.h"
#include "iap.h"
#include "ota.h"
#include <string.h>

/*============================================================*/
/* 常量定义                                                   */
/*============================================================*/
#define KEY_DEBOUNCE_MS     50      /* 按键消抖时间 */
#define KEY_WAIT_MS         3000    /* 按键等待超时 */

/*============================================================*/
/* 全局变量                                                   */
/*============================================================*/
BootMode_t g_boot_mode = MODE_BOOT_NO_UPDATE;

uint32_t s_target_bank = ADDR_APP_B;   /* 目标分区（OTA/IAP 写入目标） */

static uint32_t s_active_bank  = ADDR_APP_A;   /* 当前激活分区 */

/*============================================================*/
/* 内部函数                                                   */
/*============================================================*/

/* 获取非活跃分区 */
static uint32_t get_inactive_bank(void)
{
    return (s_active_bank == ADDR_APP_A) ? ADDR_APP_B : ADDR_APP_A;
}

/*============================================================*/
/* W25Q64 元数据读写                                          */
/*============================================================*/
/* 返回值：有效固件大小，0 表示无效 */
static uint32_t w25q64_read_meta(void)
{
    uint8_t meta[W25Q64_META_SIZE];
    W25Q64_ReadData(W25Q64_META_ADDR, meta, W25Q64_META_SIZE);

    uint32_t size = meta[4] | ((uint32_t)meta[5] << 8) |
                    ((uint32_t)meta[6] << 16) | ((uint32_t)meta[7] << 24);

    /* 参数校验 */
    if (size < APP_SIZE_MIN || size > FLASH_APP_SIZE) {
        return 0;
    }

    /* 固件头校验：栈顶地址必须在 SRAM 范围内 */
    uint8_t header[4];
    W25Q64_ReadData(W25Q64_FW_OFFSET, header, 4);
    uint32_t stack = header[0] | ((uint32_t)header[1] << 8) |
                     ((uint32_t)header[2] << 16) | ((uint32_t)header[3] << 24);

    if ((stack & SRAM_STACK_MASK) != SRAM_STACK_BASE) {
        return 0;
    }

    return size;
}

void w25q64_save_meta(uint32_t size)
{
    uint8_t meta[W25Q64_META_SIZE];
    memcpy(meta + 0, &W25Q64_FW_OFFSET, 4);
    memcpy(meta + 4, &size, 4);
    W25Q64_WriteData(W25Q64_META_ADDR, meta, W25Q64_META_SIZE);
}

/*============================================================*/
/* 分区操作（直接调用底层接口）                                */
/*============================================================*/
void erase_target_bank(void)
{
    Boot_EraseFlash(s_target_bank, 8);
}

/* W25Q64 → Flash 分区（使用 Boot_CopyFlash 从 W25Q64 读取并写入） */
void copy_w25q64_to_flash(uint32_t size)
{
    uint8_t buff[256];
    uint32_t off = 0;

    while (off < size) {
        uint16_t chunk = (size - off >= sizeof(buff)) ? sizeof(buff) : (size - off);
        W25Q64_ReadData(W25Q64_FW_OFFSET + off, buff, chunk);
        Boot_WriteFlash(s_target_bank + off, buff, chunk);
        off += chunk;
    }
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

    if (uart_recv_done && Boot_IsValidFirmware(s_target_bank)) {
        switch_bank();
    }
}

/*============================================================*/
/* W25Q64 升级模式：已有固件在 W25Q64 中                      */
/*============================================================*/
static void w25q64_upgrade(void)
{
    uint32_t size = w25q64_read_meta();
    if (size == 0) return;

    erase_target_bank();
    copy_w25q64_to_flash(size);

    if (Boot_IsValidFirmware(s_target_bank)) {
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
    if (g_boot_mode == MODE_BOOT_NO_UPDATE && !Boot_IsValidFirmware(s_active_bank)) {
        uint32_t alt = get_inactive_bank();
        if (Boot_IsValidFirmware(alt)) {
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

    Boot_CopyFlash(s_active_bank, ADDR_APP_RUN, FLASH_APP_SIZE);
    Boot_JumpToApp(ADDR_APP_RUN);
}
