#include "HCSR04.h"

// PA10(TIM1_CH3) & PA11(TIM1_CH4): 双输入捕获
void HCSR04_Init(void)
{
    // 1. 时钟使能
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // 2. GPIO 配置
    // PA10 (TIM1_CH3): 浮空输入（接 Echo）
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_1;

    // PA11: Trig 推挽输出
    GPIOA->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
    GPIOA->CRH |= GPIO_CRH_MODE11_1;  // 50MHz 输出

    // 3. 定时器基础配置
    TIM1->PSC = 72 - 1;   // 1MHz 计数频率
    TIM1->ARR = 0xFFFF;    // 最大计数值
    TIM1->CR1 &= ~TIM_CR1_DIR; // 向上计数

    // 4. TI3的输入捕获选择（关键！）
    TIM1->CCMR2 = 0;

    // 4.2 CH3: 直接映射->TI3 (PA10)
    TIM1->CCMR2 |= TIM_CCMR2_CC3S_0;
    // 4.3 CH4: 间接映射->TI3（实现间接捕获）
    TIM1->CCMR2 |= TIM_CCMR2_CC4S_1;
    
    // 4.5 预分频器(默认0:不分频)
    TIM1->CCMR2 &= ~TIM_CCMR2_IC3PSC;

    // 4.6 捕获使能(CCxE:配置输出 CCxP:配置极性)
    TIM1->CCER |=  TIM_CCER_CC3E | TIM_CCER_CC4E;

    // 4.7 极性：CH3 上升沿，CH4 下降沿
    TIM1->CCER &= ~TIM_CCER_CC3P;       // CH3: rising  
    TIM1->CCER |=  TIM_CCER_CC4P;       // CH4: falling (inverted)

    // 5. 中断使能
    TIM1->DIER |=  TIM_DIER_CC4IE; // 接收到低电平后中断通知 

    // 6. NVIC配置
    NVIC_SetPriorityGrouping(2);
    NVIC_SetPriority(TIM1_CC_IRQn, 2);
    NVIC_EnableIRQ(TIM1_CC_IRQn);

//    // 7. 启动定时器
//    TIM1->CR1 |= TIM_CR1_CEN;
}

// 全局变量
volatile uint16_t upEdge = 0;
volatile uint16_t downEdge = 0;
float distance = 0;

void TIM1_CC_IRQHandler()
{
    
    if (TIM1->SR & TIM_SR_CC4IF) 
    {
        upEdge = TIM1->CCR3;
        downEdge = TIM1->CCR4;
        
        // 清除CH4的标志位
        TIM1->SR &= ~TIM_SR_CC4IF;

        distance = ((downEdge - upEdge ) * 0.034 )/ 2;
    }
}

void HCSR04_Trigger(void)
{
    GPIOA->ODR |= GPIO_ODR_ODR11;
    Delay_us(40);  // 40-50us最佳
    GPIOA->ODR &= ~GPIO_ODR_ODR11;
}



