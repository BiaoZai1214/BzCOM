#ifndef __AT24C64_H
#define __AT24C64_H

/* 头文件 */
#include "i2c.h"
#include "usart.h"

/* 宏定义 */
#define AT24C64_ADDR_W 		0xA0
#define AT24C64_ADDR_R		0xA1

#define AT24C64_ADDR_SIZE	8
#define AT24C64_PAGE_SIZE	16

/* 数据结构 */

/* 函数声明 */
void AT24C64_Init(void);

// 读/写单字节
uint8_t AT24C64_R_Byte(uint16_t byte_addr);
void AT24C64_W_Byte(uint16_t byte_addr, uint8_t data);

// 读/写多个字节
void AT24C64_R_Bytes(uint16_t byte_addr, uint8_t *data, uint16_t len);
void AT24C64_W_Bytes(uint16_t byte_addr, uint8_t *data, uint16_t len);

// 多段写入(跨页写)


#endif
