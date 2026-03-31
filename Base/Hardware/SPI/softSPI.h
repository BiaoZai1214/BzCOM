#ifndef __SOFTSPI_H
#define __SOFTSPI_H

/* 引用头文件 */
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/* 宏定义 */
// RST		 INT		 SCK		 CS		 	MOSI	 	 MISO
// B2		 X			 A12		 A15		B8		 	 B1	

#define SCK_HIGH     (GPIOA->ODR |=  GPIO_ODR_ODR12)
#define SCK_LOW      (GPIOA->ODR &= ~GPIO_ODR_ODR12)

#define CS_HIGH      (GPIOA->ODR |=  GPIO_ODR_ODR15)
#define CS_LOW       (GPIOA->ODR &= ~GPIO_ODR_ODR15)
// 硬件SPI1的CS
//#define CS_HIGH     (GPIOA->ODR |=  GPIO_ODR_ODR4)
//#define CS_LOW      (GPIOA->ODR &= ~GPIO_ODR_ODR4)

#define MOSI_HIGH    (GPIOB->ODR |=  GPIO_ODR_ODR8)
#define MOSI_LOW     (GPIOB->ODR &= ~GPIO_ODR_ODR8)

// 读取电平信号📶
#define MISO_READ    (GPIOB->IDR & GPIO_IDR_IDR1)                                                                                                       

#define SPI_DELAY    Delay_us(2)
/* 数据结构 */

/* 函数声明 */
void SoftSPI_Init(void);

void SoftSPI_Start(void);

void SoftSPI_Stop(void);

uint8_t SoftSPI_SwapByte(uint8_t byte);

#endif
