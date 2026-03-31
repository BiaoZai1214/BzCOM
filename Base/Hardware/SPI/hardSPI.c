#include "hardSPI.h"

void hardSPI_Init(void)
{
	    // 1. 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    // 2. GPIO_MODE 
    
    // PA5: SCL 复用推挽输出 CNF-10 MODE-11
    GPIOA->CRL |=  GPIO_CRL_MODE5;
    GPIOA->CRL |=  GPIO_CRL_CNF5_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF5_0;
    
	// PA7: MOSI 复用推挽输出 CNF-10 MODE-11
    GPIOA->CRL |=  GPIO_CRL_MODE7;
    GPIOA->CRL |=  GPIO_CRL_CNF7_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF7_0;
     
    // 3.  选择SPI的工作模式0: SCK空闲0
    // 3.1 主模式
    SPI1->CR1 |= SPI_CR1_MSTR;
    
    // 3.2 软件控制片选信号,直接拉高
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;
    
    // 3.3 工作模式(默认0)
    SPI1->CR1 &= ~SPI_CR1_CPOL;
    SPI1->CR1 &= ~SPI_CR1_CPHA;
    
    // 3.4 配置时钟分配系数,波特率: BR-001 4分频 18MHz
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_0;
    
    // 3.5 设置数据帧格式
    SPI1->CR1 &= ~SPI_CR1_DFF;
    
    // 3.6 配置高位先行
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
    
    SPI1->CR1 |= SPI_CR1_SPE;
}






