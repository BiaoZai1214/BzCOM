#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

#include "usart.h"
#include "stdlib.h"
#include "string.h"

/*============================================================*/
/* Flash 分区布局                                             */
/*============================================================*/
#define ADDR_BOOT          0x08000000  // Bootloader 区
#define ADDR_APP_RUN       0x08004000  // 运行区（固件搬运目标）
#define ADDR_APP_A         0x08008000  // A 区固件
#define ADDR_APP_B         0x0800C000  // B 区固件
#define ADDR_FLASH_END     0x08010000  // Flash 结束地址 (64KB)

#define FLASH_BOOT_SIZE    (16 * 1024) // Bootloader 大小 16KB
#define FLASH_APP_SIZE     (16 * 1024) // 每个 APP 区大小 16KB

/*============================================================*/
/* SRAM 地址校验                                              */
/*============================================================*/
#define SRAM_STACK_BASE    0x20000000  // SRAM 基地址（CMSIS 定义）
#define SRAM_STACK_MASK    0xFFFF0000  // 栈顶地址高 16 位掩码

/*============================================================*/
/* AT24C64 存储地址                                          */
/*============================================================*/
#define ADDR_ACTIVE_BANK   0x13        // 激活分区地址

/*============================================================*/
/* 激活分区标志                                               */
/*============================================================*/
#define FLAG_BANK_A        0xAA        // A 区激活
#define FLAG_BANK_B        0xBB        // B 区激活

/*============================================================*/
/* 出厂固件地址                                               */
/*============================================================*/
#define ADDR_FACTORY       ADDR_APP_RUN // 出厂固件地址（现作为运行区）

/*============================================================*/
/* 公共接口                                                   */
/*============================================================*/
uint8_t Boot_JumpToApp(uint32_t addr);
void    Boot_EraseFlash(uint32_t addr, uint16_t pages);
void    Boot_WriteFlash(uint32_t addr, const uint8_t *data, uint32_t len);
void    Boot_CopyFlash(uint32_t src, uint32_t dst, uint32_t size);
uint32_t Boot_GetActiveBank(void);
void    Boot_SetActiveBank(uint32_t addr);
void    Boot_StartUartIap(void);
uint8_t Boot_IsValidFirmware(uint32_t addr);
void    UART_ClearError(void);

#endif
