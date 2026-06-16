#include "SoftI2C.h"

#define ADDRESS     0x7A


// SCL: PA6  SDA: PA7 
void SoftI2C_Init(void)
{
    // 1. Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    // 2.1 GPIO MODE: 通用开漏(CNF-01 MODE-11)
    GPIOB->CRL |=  (GPIO_CRL_MODE6  | GPIO_CRL_MODE7);
    GPIOB->CRL &= ~(GPIO_CRL_CNF6_1 | GPIO_CRL_CNF7_1);
    GPIOB->CRL |=  (GPIO_CRL_CNF6_0 | GPIO_CRL_CNF7_0);
}

// 拉高SCL采样SDA 高->低 跳变
void SoftI2C_Start(void)
{
    SCL_HIGH;
    SDA_HIGH;
    I2C_DELAY;
    //SDA跳变
    SDA_LOW;
    I2C_DELAY;
}
// 拉高SCL采样SDA 低->高 跳变
void SoftI2C_Stop(void)
{
    SCL_HIGH;
    SDA_LOW;
    I2C_DELAY;  
    //SDA跳变
    SDA_HIGH;
    I2C_DELAY;
}

void SoftI2C_ACK(void)
{
    // 1. SCL拉低,SDA拉高: 准备发送数据
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;
    
    // 2. SCL不变,SDA拉低 
    SDA_LOW;
    I2C_DELAY;
    
    // 3. SCL拉高,SDA不变:开始采样数据: ACK(0)
    SCL_HIGH;
    I2C_DELAY;
    
    // 4. SCL拉低,SDA不变
    SCL_LOW;
    I2C_DELAY;
    
    // 5.SDA拉高,释放总线占用
    SDA_HIGH;
    I2C_DELAY;
}
void SoftI2C_NACK(void)
{
    // 1. SCL拉低,SDA拉高: 准备发送数据
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;
    
    // 2. SCL拉高,SDA不变: 开始采样数据: NACK(1)
    SCL_HIGH;
    I2C_DELAY;
    
    // 3. SCL拉低,SDA不变
    SCL_LOW;
    I2C_DELAY;
}

uint8_t SoftI2C_Wait4Ack(void)
{
    // 1. 拉低SCL,拉高SDA: 释放数据总线
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;
    
    // 2. SCL拉高: 开始采样数据
    SCL_HIGH;
    I2C_DELAY;
    
    // 3. 读SDA的电平信号(1:结束, 0:继续)
    uint16_t ack = READ_SDA;
    I2C_DELAY;
    
    // 4.  SCL拉低: 结束采样
    SCL_LOW;
    I2C_DELAY;
    
    return ack ? NACK : ACK;
}

// 发送1个字节
void SoftI2C_SendByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i ++)
    {
        // 1. SCL & SDA 都拉低变化数据
        SCL_LOW;
        SDA_LOW;
        I2C_DELAY;
        
        // 2. I2C是高位优先,SDA取字节的最高位发送
        if(byte & 0x80)
        {
            SDA_HIGH;
        }
        else
        {
            SDA_LOW;
        }
        I2C_DELAY;
        
        // 3. SCL拉高:数据采样
        SCL_HIGH;
        I2C_DELAY;
        
        // 4. SCL拉低,结束数据采样
        SCL_LOW;
        I2C_DELAY;
        
        // 5. 数据左移1位
        byte <<= 1;
    }
}

uint8_t SoftI2C_ReadByte(void)
{
    // 定义一个变量保存接收的数据
    uint8_t data = 0;
    
    for (uint8_t i = 0; i < 8; i ++)
    {
        // 1. SCL拉低,等待数据翻转
        SCL_LOW;
        I2C_DELAY;
        
        // 2. SCL拉高,开始采样
        SCL_HIGH;
        I2C_DELAY;
        
        // 3. 数据采样
        data <<= 1;
        if (READ_SDA)
        {
            data |= 0x01;   // 先存最低位,每次左移1位
        }
        
        // 4. SCL拉低,结束采样
        SCL_LOW;
        I2C_DELAY;
    }
    return data;
}

