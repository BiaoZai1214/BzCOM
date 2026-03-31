#ifndef __USART_H
#define __USART_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "stdio.h"

/* 函数声明 */
extern uint8_t buffer[];
extern uint8_t Size;
extern uint8_t isOver;

void USART1_Init(void);

void USART1_SendChar(uint8_t ch);

uint8_t USART1_ReceiveChar(void);

// 发送字符串
void USART1_SendString(uint8_t *str, uint8_t Size);

// 接收字符串
void USART1_ReceiveString(uint8_t buffer[], uint8_t *Size);

#endif


