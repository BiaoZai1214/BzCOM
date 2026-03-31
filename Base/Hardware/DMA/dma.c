#include "dma.h"

// 存储器到存储器
void DMA1_Init(void)
{
    // 1. 开启时钟
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    
    // 2.  DMA相关配置
    // 2.1 数据传输方向:  从外设读
//    // 存储器到存储器
    DMA1_Channel1->CCR |= DMA_CCR1_MEM2MEM;
    DMA1_Channel7->CCR |= DMA_CCR7_DIR;
    
    // 2.2 数据宽度: 默认8位 - 00
    DMA1_Channel7->CCR &= ~DMA_CCR1_PSIZE;
    DMA1_Channel7->CCR &= ~DMA_CCR1_MSIZE;
    
    // 2.3 地址自增: 开启自增 (串口地址不能自增,默认不自增)
    DMA1_Channel7->CCR &= ~DMA_CCR7_PINC;
    DMA1_Channel7->CCR |= DMA_CCR7_MINC;
    
    // 2.4 开启数据传输完成中断标志
    DMA1_Channel7->CCR |= DMA_CCR7_TCIE;
    
    // 2.5 使能串口的DMA传输功能(串口)
    USART2->CR3 |= USART_CR3_DMAT;
    
//    // 2.6 循环传输功能(可选):
//    DMA1_Channel7->CCR |= DMA_CCR7_CIRC;
    
    // 3.  NVIC配置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(DMA1_Channel7_IRQn, 3);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    
}

// 数据传输
void DMA1_Transmit(uint32_t srcAddr, uint32_t destAddr, uint16_t dataLen)
{
    // 1. 设置外设地址
    DMA1_Channel7->CPAR = destAddr;
    
    // 2. 设置存储器地址
    DMA1_Channel7->CMAR = srcAddr;
    
    // 3. 设置传输的数据量
    DMA1_Channel7->CNDTR = dataLen;
    
    // 4. 开启DMA通道,开始传输数据
    DMA1_Channel7->CCR |= DMA_CCR7_EN;
  
}

void DMA1_Channel7_IRQHandler(void)
{
    // 判断中断标志位
    if(DMA1->ISR & DMA_ISR_TCIF7)
    {
        // 清除发送完成中断标志位
        DMA1->IFCR |= DMA_IFCR_CTCIF7;
        // 关闭DMA通道
        DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
    }
}

// 在主函数中的调用
//// 定义全局变量,表示DMA数据传输完成
//uint8_t isFinished = 0;

////// 定义全局常量,放置在ROM中,作为数据源
////const uint8_t src[] = {10, 20, 30, 40};

//// 定义一个变量数组,放置在RAM中,存储接收到的数据
//uint8_t src[] = "你好呀,我叫标仔";

//int main()
//{
//    USART2_Init();
//    DMA1_Init();
//    
//    printf("Hello,World!\n");
//    
////    // 打印变量和常量地址
////    printf("Rom = %p,  Ram = %p\n",src, dest);
//    
//    // 开启DMA通道进行传输
//    DMA1_Transmit((uint32_t)src, (uint32_t)&(USART2->DR), 30);
//    
//    while(1)
//    {   
//        
//    }
//}
