#ifndef __ADC_DMA_H
#define __ADC_DMA_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header

/* 宏定义 */

/* 数据结构 */

/* 函数声明 */
void ADC1_DMA_Init(void);

void ADC1_DMA_StartConvert(uint32_t destAddr, uint8_t len);

//// 返回转换后的模拟电压值
//double ADC1_DMA_ReadV(void);

#endif
