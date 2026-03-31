#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stm32f10x.h"                  // Device header

#define USE_HORIZONTAL 0  //显示方向 0或1为竖屏 2或3为横屏

#define LCD_W 240
#define LCD_H 240


//-----------------LCD端口定义---------------- 

// RST		 DC		 CS		 BLK
// PB0		 B1		 B10	 3.3

#define LCD_RST_HIGH     (GPIOB->ODR |=  GPIO_ODR_ODR0)
#define LCD_RST_LOW      (GPIOB->ODR &= ~GPIO_ODR_ODR0)

#define LCD_DC_HIGH      (GPIOB->ODR |=  GPIO_ODR_ODR1)
#define LCD_DC_LOW       (GPIOB->ODR &= ~GPIO_ODR_ODR1)

#define LCD_CS_HIGH      (GPIOB->ODR |=  GPIO_ODR_ODR10)
#define LCD_CS_LOW       (GPIOB->ODR &= ~GPIO_ODR_ODR10)


void LCD_GPIO_Init(void);//初始化GPIO
void LCD_WR_DATA(u8 data);//写入一个字节
void LCD_WR_DOUBLE(u16 data);//写入两个字节
void LCD_WR_CMD(u8 data);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_SendData16(uint16_t data);
void LCD_Init(void);//LCD初始化
#endif




