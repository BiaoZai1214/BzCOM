#ifndef __TIM1_H
#define __TIM1_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "stdio.h"
#include "led.h"

/* 宏定义 */

/* 数据结构 */

/* 函数声明 */
void TIM1_Init(void);

// 定时的开关包装成函数
void TIM1_Start(void);
void TIM1_Stop(void);

// 设置占空比的函数
void TIM3_SetDutyCycle(uint8_t dutyCycle);

#endif
