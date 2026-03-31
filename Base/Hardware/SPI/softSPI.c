#include "softSPI.h"

// SCK    CS     MOSI    MISO
// PA12   PA15   PB3     PB1                                                   PA15

void SoftSPI_Init(void)
{
    // 0. ★★★ 关键：释放 JTAG 引脚 ★★★
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // 关闭 JTAG，保留 SWD
    AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
              
    // 1. 使能时钟🕰
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    // 2. GPIO_MODE 
    // PA12 PA15: 通用推挽输出 CNF: 00 MODE: 11
    GPIOA->CRH &= ~(GPIO_CRH_CNF12  | GPIO_CRH_CNF15);
    GPIOA->CRH |=  (GPIO_CRH_MODE12 | GPIO_CRH_MODE15); 
	
	// PB8: (MOSI)通用推挽输出 CNF: 00 MODE: 11
    GPIOB->CRH &= ~GPIO_CRH_CNF8;
    GPIOB->CRH |= GPIO_CRH_MODE8;
    
    // PB1: (MISO) 浮空输入 CNF:01, MODE:00;
    GPIOB->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1); // 先清零
	GPIOB->CRL |= GPIO_CRL_CNF1_1; 
	
    // 3. 选择SPI的工作模式0: SCK空闲0
    SCK_LOW;
    
    // 4. 片选拉高
    CS_HIGH;
    
    // 5. 延时
    SPI_DELAY;
}

void SoftSPI_Start(void)
{
    CS_LOW;
}

void SoftSPI_Stop(void)
{
    CS_HIGH;
}

uint8_t SoftSPI_SwapByte(uint8_t byteSend)
{
    // 定义变量保存接收到的字节
    uint8_t byteReceive = 0x00;
    
    // 用循环,依次交换8位数据
    for (uint8_t i = 0; i < 8; i++)
    {
        // 1. 先准备要发送的数据(最高位),送到MOSI
        if (byteSend & (0x80 >> i))
            MOSI_HIGH;
        else 
            MOSI_LOW;
        
        // 2. 拉高时钟信号,形成一个上升沿
        SCK_HIGH;
        SPI_DELAY;
        
        // 3. 在MISO上采样Flash的数据
        if(MISO_READ)
        {
            byteReceive |= (0x80 >> i);
        }
        
        //4. 拉低时钟,为下次数据传输做准备
        SCK_LOW;
        SPI_DELAY;
    }
    return byteReceive;
}
