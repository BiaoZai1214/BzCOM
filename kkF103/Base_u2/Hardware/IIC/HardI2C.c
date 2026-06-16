#include "HardI2C.h"


void HardI2C_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    // 2. GPIO工作模式: 复用开漏 (CNF:11, MODE:11)
    GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_MODE7 | 
                   GPIO_CRL_CNF6  | GPIO_CRL_CNF7 );
    
    // 3. I2C1配置
    // 3.1 硬件工作模式(默认0,不用配)
    I2C1->CR1 &= ~I2C_CR1_SMBUS;
    I2C1->CR1 &= ~I2C_CCR_FS;
    
    // 3.2 选择输入的时钟频率
    I2C1->CR2 |= 36;
    
    // 3.3 配置CCR,对应传输速度100kb/s,SCL高电平时间5us
    I2C1->CCR |= 180;
    
    // 3.4 配置TRISE,SCL上升沿最大周期 +1
    I2C1->TRISE |= 36 + 1;
    
    // 3.5使能I2C1 模块
    I2C1->CR1 |= I2C_CR1_PE;
}

uint8_t HardI2C_Start(void)
{
    // 起始信号:需要结合SR1_SB位,判断当前是否正常通讯
    I2C1->CR1 |= I2C_CR1_START;
    
    uint16_t TimeOut = 0xffff;  // 引入计时
    while((I2C1->SR1 & I2C_SR1_SB) == 0 && TimeOut)
    {
        TimeOut --;
    }
    // 退出while循环: 事件退出 OR 时间耗尽退出
    return TimeOut ? OK : FAIL; 
}

void HardI2C_Stop(void)
{
    // 延时生效:等字节传输结束
    I2C1->CR1 |= I2C_CR1_STOP;
}

void HardI2C_ACK(void)
{
    I2C1->CR1 |= I2C_CR1_ACK;
}

void HardI2C_NACK(void)
{
    I2C1->CR1 &= ~I2C_CR1_ACK;
}

uint8_t HardI2C_SendAddr(uint8_t addr)
{
    // 把要发送的地址给到DR
    I2C1->DR = addr;
    
    // 等待ACK应答
    uint16_t TimeOut = 0xffff;
    while((I2C1->SR1 & I2C_SR1_ADDR) == 0 && TimeOut)
    {
        TimeOut --;
    }
    // 访问SR2,清除ADDR标志位
    if(TimeOut > 0)
    {
        I2C1->SR2;
    }
    return TimeOut ? addr : FAIL;
}

uint8_t HardI2C_SendByte(uint8_t byte)
{
    // 1. 等待TXE空: 上一个数据已发完
    uint16_t TimeOut = 0xffff;
    while((I2C1->SR1 & I2C_SR1_TXE) == 0 && TimeOut)
    {
        TimeOut --;
    }
    
    // 2.将要发送的字节放入DR
    I2C1->DR = byte;
    
    // 3.等待ACK应答
    TimeOut = 0xffff;
    while((I2C1->SR1 & I2C_SR1_BTF) == 0 && TimeOut)
    {
        TimeOut --;
    }
    return TimeOut ? OK : FAIL;
}

// 发送设备地址
uint8_t HardI2C_ReadByte(void)
{
    // 1. 等待RDR为满: 数据帧已接收
    uint16_t TimeOut = 0xffff;
    while((I2C1->SR1 & I2C_SR1_RXNE) == 0 && TimeOut)
    {
        TimeOut --;
    }
    
    // 2. 将收到的字节数据返回
    return TimeOut ? I2C1->DR : FAIL;
}
