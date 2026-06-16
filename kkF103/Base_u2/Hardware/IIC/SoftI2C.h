#ifndef __SOFTI2C_H
#define __SOFTI2C_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "Delay.h"  // (要引入一个ms级延时函数)

/* 宏定义 */
#define ACK         0
#define NACK        1

#define SCL_HIGH    (GPIOB->ODR |=  GPIO_ODR_ODR6)
#define SCL_LOW     (GPIOB->ODR &= ~GPIO_ODR_ODR6)
#define SDA_HIGH    (GPIOB->ODR |=  GPIO_ODR_ODR7)
#define SDA_LOW     (GPIOB->ODR &= ~GPIO_ODR_ODR7)

// 读操作
#define READ_SDA    (GPIOB->IDR & GPIO_IDR_IDR7)

// 定义操作的基本延时
#define I2C_DELAY   Delay_us(1)  // 高速IIC,可以调高延时
   
/* 函数声明 */
void SoftI2C_Init(void);
void SoftI2C_Start(void);
void SoftI2C_Stop(void);

void SoftI2C_ACK(void);     // 发ACK
void SoftI2C_NACK(void);    // 发NACK

uint8_t SoftI2C_Wait4Ack(void);

void SoftI2C_SendByte(uint8_t byte);

uint8_t SoftI2C_ReadByte(void);

#endif


