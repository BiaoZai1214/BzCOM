#include "usart.h"

uint8_t Size = 0;
uint8_t buffer[20] = {0};
uint8_t isOver = 0;

void USART2_Init(void)
{
    // 1.Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    
    // 2.GPIO Mode  
    // PA2(复用推挽: CNF:10 MODE:11) 
    GPIOA->CRL |=  GPIO_CRL_MODE2;
    GPIOA->CRL |=  GPIO_CRL_CNF2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;
    // PA3(浮空输入: CNF:01 MODE:00)
    GPIOA->CRL &= ~GPIO_CRL_MODE3;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;
    GPIOA->CRL |=  GPIO_CRL_CNF3_0;
    
    // 3.1 波特率设置(apb1:36MHz下BRR:115200 = 0x139)
    USART2->BRR = 0x139;
    
    // 3.2 其他配置: 设置数据帧格式(默认 )
    USART2->CR1 &= ~USART_CR1_M;		// 位数
    USART2->CR1 &= ~USART_CR1_PCE;		// 校验位
    USART2->CR2 &= ~USART_CR2_STOP;		// 停止位
    
    // 3.3 开启中断使能
    USART2->CR1 |= (USART_CR1_IDLEIE | USART_CR1_RXNEIE);
    
    // 4.1 NVIC配置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(USART2_IRQn, 3); // 
    NVIC_EnableIRQ(USART2_IRQn);      //
	
	// 4.2 收发使能
    USART2->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

// 发送字符
void USART2_SendChar(uint8_t ch)
{
    // 等待发送完成(TXE=0: TDR还在发数据,等TXE=1)
    while((USART2->SR & USART_SR_TXE) == 0);
    
    // 向DR写入新数据(写DR会清TXE为0)
    USART2->DR = ch;
    
    while((USART2->SR & USART_SR_TC) == 0);
}

// 发送字符串: 重复发送n个字符数据
void USART2_SendString(uint8_t *str, uint8_t Size)
{
    for(uint8_t i = 0; i< Size; i++)
    {
        USART2_SendChar(str[i]);
    } 
}

// 接收字符
uint8_t USART2_ReceiveChar(void)
{
    // 等待接收完成(RXNE=0: RDR没接收完1个字节数据,等RXNE=1)
    while((USART2->SR & USART_SR_RXNE) == 0)
    {
        // 增加判断空闲帧的条件
        if(USART2->SR & USART_SR_IDLE)
        {
            return 0;
        }
    }
    
    // 返回DR寄存器的值到函数(读DR会清RXNE为0)
    return USART2->DR;
}

// 接收字符串
void USART2_ReceiveString(uint8_t *buffer, uint8_t *Size)
{
    uint8_t i = 0;
    
    // IDLE接收到 空闲帧 后 置1退出循环
    while ((USART2->SR & USART_SR_IDLE) == 0)
    {
        buffer[i] = USART2_ReceiveChar();
        i ++;
    } 
    
    // 软件(手动)清除IDLE位
//    USART2->SR;
    USART2->DR;
    
    *Size = i - 1;
}

void USART2_IRQHandler(void)
{
    // 如果是RXNE触发: 接收一个字符完成
    if (USART2->SR & USART_SR_RXNE)
    {
        buffer[Size] = USART2->DR;
        Size ++;
    }
    // 如果整体接收完成
    else if (USART2->SR & USART_SR_IDLE)
    {
        USART2->SR;
        USART2->DR;     // 软件(手动)清除IDLE
        
        isOver = 1;
    }
}

// 重写fputc
int fputc(int ch, FILE * file)
{
    USART2_SendChar(ch);
    return ch;
}

