#ifndef __USART_H
#define __USART_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "stdio.h"

/* 函数声明 */
extern uint8_t buffer[];
extern uint8_t Size;
extern uint8_t isOver;

void USART2_Init(void);

void USART2_SendChar(uint8_t ch);

uint8_t USART2_ReceiveChar(void);

// 发送字符串
void USART2_SendString(uint8_t *str, uint8_t Size);

// 接收字符串
void USART2_ReceiveString(uint8_t buffer[], uint8_t *Size);

#endif


