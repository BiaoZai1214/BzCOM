/**
 * @file app_bootloader.h
 * @brief Bootloader 启动流程状态机
 */
#ifndef __APP_BOOTLOADER_H
#define __APP_BOOTLOADER_H

#include "at24c64.h"
#include "w25q64.h"
#include "bootloader.h"

#define KEY_DEBOUNCE_MS     50
#define KEY_WAIT_MS         5000

/* AT24C64 存储布局 */
#define ADDR_UPDATE_FLAG   0x10    // 更新标志地址
#define ADDR_CHECK_KEY     0x11    // 密钥地址（低字节）
#define ADDR_CHECK_KEY_H   0x12    // 密钥地址（高字节）
#define CHECK_KEY          0x5A6B // 密钥校验值

/* 启动模式 */
typedef enum {
    MODE_BOOT_UPDATE      = 0x01, // W25Q64 升级
    MODE_BOOT_NO_UPDATE   = 0x02, // 直接启动
    MODE_BOOT_RESET       = 0x03, // 恢复出厂
    MODE_BOOT_UART_IAP    = 0x04, // UART IAP 直写
    MODE_BOOT_OTA_SELECT  = 0x05, // OTA 模式选择
    MODE_BOOT_OTA         = 0x07  // UART OTA
} BootMode_t;

/* W25Q64 固件存储 */
#define W25Q64_FW_OFFSET   0x10000 // 固件存储偏移
#define W25Q64_META_ADDR   0       // 元数据地址
#define W25Q64_META_SIZE   8       // 元数据大小

/* 固件参数校验 */
#define APP_SIZE_MIN       500     // 最小固件大小
#define SRAM_MAX           0x20005000 // SRAM 最大地址

/* 公共接口 */
void App_bootloader_check_update(void);
void App_bootloader_check_default(void);
void App_bootloader_update(void);
void App_bootloader_copy_and_jump(void);

/* OTA 模块共享接口（定义在 App_bootloader.c）*/
extern BootMode_t g_boot_mode;
extern uint32_t   s_target_bank;
extern uint8_t    is_valid_firmware(uint32_t addr);
extern void       w25q64_save_meta(uint32_t size);
extern void       copy_w25q64_to_flash(uint32_t size);
extern void       switch_bank(void);

/* OTA 模块接口（定义在 ota.c） */
extern void OTA_Select(void);

/* IAP 模块接口（定义在 App_bootloader.c） */
extern void do_uart_iap(void);

#endif
