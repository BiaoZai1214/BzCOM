#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "usart.h"
#include "stdlib.h"
#include "string.h"
#define BOOTLOADER_UART_REC_BUFF_LEN 512


// 程序写入的起始位置 => A区起始位置 假设B区16K  0x4000 A区(512-16)K  0x7C000
#define APP_START_ADDR 0x08004000

#define APP_END_ADDR 0x0800C000
#define STACK_ADDR 0X20000000

void Int_bootloader_receive_app(void);

uint8_t Int_bootloader_jump_to_app(void);

void Int_bootloader_erase_flash(uint32_t page_addr,uint16_t pages);

#endif
