#include "adc_dma.h"

void ADC1_DMA_Init(void)
{
    // 1. 时钟配置
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    // CFGR(RCC的寄存器): ADCPRE - 10 (6分频 12MHz)
    RCC->CFGR |=  RCC_CFGR_ADCPRE_1;
    RCC->CFGR &= ~RCC_CFGR_ADCPRE_0;
    
    // 2. GPIO工作模式 PA4 PA5 模拟输入 CNF: 00 MODE: 00
    GPIOA->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    
    // 3.  ADC配置
    // 3.1 工作模式: 开启扫描模式,多通道
    ADC1->CR1 |= ADC_CR1_SCAN;
    
    // 3.2 启用连续转换模式(列表循环)
    ADC1->CR2 |= ADC_CR2_CONT;  
    
    // 3.3 数据对齐方式(默认右对齐)
    ADC1->CR2 &= ~ADC_CR2_ALIGN;
    
    // 3.4a 设置通道4的采样周期,110: 71.5个时钟周期
    ADC1->SMPR2 |=  ADC_SMPR2_SMP4;
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP4_0;
    
    // 3.4b 设置通道5的采样周期,001: 7.5个时钟周期
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP5;
    ADC1->SMPR2 |=  ADC_SMPR2_SMP5_0;
    
    // 3.5 规则组通道序列
    //  .a 规则组中的通道数: L
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |=  ADC_SQR1_L_0;
    
    //  .b 把通道4保存到序列中的第一位
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= 4 << 0;
    
    //  .b 把通道5保存到序列中的第二位
    ADC1->SQR3 &= ~ADC_SQR3_SQ2;
    ADC1->SQR3 |= 5 << 5;
    
    // 3.6 选择软件触发AD转换 (111)
//    ADC1->CR2 |= ADC_CR2_EXTTRIG;   // 外部触发模式
//    ADC1->CR2 |= ADC_CR2_EXTSEL;    // 选择SWSTART控制AD转换
    
    // 1. 使能DMA时钟
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    
    // 2. 设置通道1的传输方向,外设->内存(DIR = 0)
    DMA1_Channel1->CCR &= ~ DMA_CCR1_DIR;
    
    // 3. 数据宽度,16位: SIZE:01
    DMA1_Channel1->CCR &= ~DMA_CCR1_PSIZE_1;
    DMA1_Channel1->CCR |=  DMA_CCR1_PSIZE_0;
    DMA1_Channel1->CCR &= ~DMA_CCR1_MSIZE_1;
    DMA1_Channel1->CCR |=  DMA_CCR1_MSIZE_0;
    
    // 4. 地址自增,外设不增,内存地址自增
    DMA1_Channel1->CCR &= ~DMA_CCR1_PINC;
    DMA1_Channel1->CCR |=  DMA_CCR1_MINC;
    
    // 5. 开启循环模式
    DMA1_Channel1->CCR |= DMA_CCR1_CIRC;
    
    // 6. 开启DMA模式
    ADC1->CR2 |= ADC_CR2_DMA;
    
    
    
}

// 开启转换(带DMA)
void ADC1_DMA_StartConvert(uint32_t destAddr, uint8_t len)
{
    // 0. DMA配置源地址和目的地址 ,以及数据长度
    DMA1_Channel1->CPAR = (uint32_t) & (ADC1->DR);
    DMA1_Channel1->CMAR = destAddr;
    DMA1_Channel1->CNDTR = len;
    
    // 1. 上电唤醒
    ADC1->CR2 |= ADC_CR2_ADON;
    
    DMA1_Channel1->CCR |= DMA_CCR1_EN;
    
    // 2. 执行校准
    ADC1->CR2 |= ADC_CR2_CAL;
    // 等待校准完成
    while(ADC1->CR2 & ADC_CR2_CAL);
    
    // 3. 启动转换(软件启动)
//    ADC1->CR2 |= ADC_CR2_SWSTART;
    ADC1->CR2 |= ADC_CR2_ADON;   // ADON:1->1可以直接启动转换(规则组)
    
    // 4. 等待全部转换完成
    while((ADC1->SR & ADC_SR_EOC) == 0);
    
}


// 主函数的定义和调用
//// 定义数组转换后的信号值
//uint16_t data[2] = {0};


////int main()
//{
//    USART2_Init();
//    ADC1_DMA_Init();
//    
//    printf("Hello,World!\n");
//    
//    // 
//    ADC1_DMA_StartConvert((uint32_t)data, 2);
//    
//    while(1)
//    {   
//        printf("V1 = %.2f , V2 = %.2f\n", data[0] * 3.3 / 4095, data[1] * 3.3 / 4095);
//        Delay_ms(1000);
//    }
//}


