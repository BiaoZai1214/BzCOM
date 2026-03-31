#include "adc.h"

void ADC1_Init(void)
{
    // 1. 时钟配置
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    // CFGR(RCC的寄存器): ADCPRE - 10 (6分频 12MHz)
    RCC->CFGR |=  RCC_CFGR_ADCPRE_1;
    RCC->CFGR &= ~RCC_CFGR_ADCPRE_0;
    
    // 2. GPIO工作模式 PA5 模拟输入 CNF: 00 MODE: 00
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    
    // 3.  ADC配置
    // 3.1 工作模式: (默认禁用扫描模式)
    ADC1->CR1 &= ~ADC_CR1_SCAN;
    
    // 3.2 启用连续转换模式(单曲循环)
    ADC1->CR2 |= ADC_CR2_CONT;  
    
    // 3.3 数据对齐方式(默认右对齐)
    ADC1->CR2 &= ~ADC_CR2_ALIGN;
    
    // 3.4 设置通道5的采样周期,001: 7.5个时钟周期
    ADC1->SMPR2 &= ~ADC_SMPR2_SMP5;
    ADC1->SMPR2 |=  ADC_SMPR2_SMP5_0;
    
    // 3.5 规则组通道序列
    //  .a 规则组中的通道数: L
    ADC1->SQR1 &= ~ADC_SQR1_L;
    
    //  .b 把通道5保存到序列中的第一位
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= 5 << 0;
    
    // 3.6 选择软件触发AD转换 (111)
    ADC1->CR2 |= ADC_CR2_EXTTRIG;   // 外部触发模式
    ADC1->CR2 |= ADC_CR2_EXTSEL;    // 选择SWSTART控制AD转换
}

void ADC1_StartConvert(void)
{
    // 1. 上电唤醒
    ADC1->CR2 |= ADC_CR2_ADON;
    
    // 2. 执行校准
    ADC1->CR2 |= ADC_CR2_CAL;
    // 等待校准完成
    while(ADC1->CR2 & ADC_CR2_CAL);
    
    // 3. 启动转换(软件启动)
    ADC1->CR2 |= ADC_CR2_SWSTART;
    // ADC1->CR2 |= ADC_CR2_ADON;   // ADON:1->1可以直接启动转换(规则组)
    
    // 4. 等待转换完成
    while((ADC1->SR & ADC_SR_EOC) == 0);
    
}


// 返回转换后的模拟电压值
double ADC1_ReadV(void)
{
    return ADC1->DR * 3.3 / 4095;   
}



