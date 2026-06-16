#ifndef __DMA_H
#define __DMA_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include <stdint.h>
/* 宏定义 */
extern uint8_t isFinished;
/* 数据结构 */

/* 函数声明 */
void DMA1_Init(void);
void DMA1_Transmit(uint32_t srcAddr, uint32_t destAddr, uint16_t dataLen);

#endif
