#ifndef __HARDI2C_H
#define __HARDI2C_H

#include "stm32f10x.h"

// 宏定义
#define OK      1
#define FAIL    0

// 初始化
void HardI2C_Init(void);

// 发出起始信号
uint8_t HardI2C_Start(void);

// 设置发出停止信号
void HardI2C_Stop(void);

// 主机设置使能应答信号
void HardI2C_Ack(void);

// 主机设置使能非应答信号
void HardI2C_Nack(void);

// 主机发送设备地址，并等待应答
uint8_t HardI2C_SendAddr(uint8_t addr);

// 主机发送一个字节的数据（写入），并等待应答
uint8_t HardI2C_SendByte(uint8_t byte);

// 主机从EEPROM接收一个字节的数据（读取）
uint8_t HardI2C_ReadByte(void);

#endif
