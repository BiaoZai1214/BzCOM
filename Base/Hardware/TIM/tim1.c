#include "tim1.h"

// PA10(TIM1_CH3) & PA11(TIM1_CH4): 双输入捕获
void TIM1_Init(void)
{
    // 1. 时钟使能
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // 2. GPIO 配置

    // PA10: TIM1_CH3 复用推挽(CNF-10  MODE-11)
    GPIOA->CRH |=  GPIO_CRH_CNF10_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF10_0;
    GPIOA->CRH |=  GPIO_CRH_MODE10;

    // 3. 定时器基础配置
    TIM1->PSC = 7200 - 1;         // 得到10000 Hz
    TIM1->ARR = 5000 - 1;         // 0.5s产生一次溢出
    TIM1->CR1 &= ~TIM_CR1_DIR;    // 向上计数
    
    // 3.2 重复计数
    TIM1->RCR = 5 - 1;

    // 4. 输出通道部分
    
    // 4.1 通道3位输出模式
    TIM1->CCMR2 &= ~TIM_CCMR2_CC3S;
    
    // 4.2 通道3为 PWM1 模式: OC3M - 110
    TIM1->CCMR2 |=  TIM_CCMR2_OC3M_2;
    TIM1->CCMR2 |=  TIM_CCMR2_OC3M_1;
    TIM1->CCMR2 &= ~TIM_CCMR2_OC3M_0;
    
    // 4.3 配置CCR,占空比:50%
    TIM1->CCR3 = 2500;
    
    // 4.4 配置极性
    TIM1->CCER &= ~TIM_CCER_CC3P;
    
    // 4.5 产生一个更新事件,刷新寄存器的值
    TIM1->CR1 |= TIM_CR1_URS;   //如果使能了中断或请求DMA,则计数器溢出才产生更新中断
    TIM1->EGR |= TIM_EGR_UG;
//    TIM1->SR &= ~TIM_SR_UIF;  // 这里是产生更新中断后直接删除标志位
    
    // 4.5 通道3使能
    TIM1->CCER |= TIM_CCER_CC3E;
    
    // 4.6 主输出使能
    TIM1->BDTR |= TIM_BDTR_MOE;
    
    // 5. 中断功能
    
    // 5.1 更新中断使能: 在溢出5次后产生的更新中断
    TIM1->DIER |= TIM_DIER_UIE;
    
    // 5.2 NVIC配置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(TIM1_UP_IRQn, 3);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    
}


void TIM1_Start(void)
{
    TIM1->CR1 |= TIM_CR1_CEN;
}

void TIM1_Stop(void)
{
    TIM1->CR1 &= ~TIM_CR1_CEN;
}

// 中断服务函数
void TIM1_UP_IRQHandler(void)
{
    //printf("进入中断");
    // 清除中断标志位
    TIM1->SR &= ~TIM_SR_UIF;
    
    // 重复计数器到下溢,触发更新中断,停掉定时器
    TIM1_Stop();
}



