#ifndef __HCSR04_H
#define __HCSR04_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "tim1.h"
/* 宏定义 */

/* 数据结构 */

/* 函数声明 */
void HCSR04_Init(void);

void TIM1_Start(void);
void TIM1_Stop(void);

void HCSR04_Trigger(void);


#endif
