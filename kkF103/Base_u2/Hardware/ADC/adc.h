#ifndef __ADC_H
#define __ADC_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header

/* 宏定义 */

/* 数据结构 */

/* 函数声明 */
void ADC1_Init(void);

void ADC1_StartConvert(void);

// 返回转换后的模拟电压值
double ADC1_ReadV(void);

#endif
