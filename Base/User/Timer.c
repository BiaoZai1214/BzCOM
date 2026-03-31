#include "stm32f10x.h"                  // Device header
#include "Timer.h"

// # Timer2给按键状态机做一个时钟基准
void Timer2_Init(void)
{
    // 1. 使能 TIM2 时钟（APB1）
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. 配置 TIM2 基本参数（寄存器直接操作）
    // DIER: 使能更新中断
    TIM2->DIER |= TIM_DIER_UIE;  // Update Interrupt Enable

    TIM2->PSC = 72-1;   // 预分频
    TIM2->ARR = 1000-1;    // 计数周期

    // CR1: 向上计数（稍后启动）
    TIM2->CR1 = TIM_CR1_ARPE;  // 可选：使能 ARR 预装载（与库行为一致）
    // 此时 CEN=0，未启动

    // 清除更新标志（避免刚配置就进中断）
    TIM2->SR &= ~TIM_SR_UIF;  // 或直接写 0

    // 3. 配置分组 (NVIC相当于在内核,所以直接用cm3定义库的函数)
    NVIC_SetPriorityGrouping(2);
    NVIC_SetPriority(TIM2_IRQn, 2); // 
    
    NVIC_EnableIRQ(TIM2_IRQn);      // 
    
    // 4. 启动定时器
    TIM2->CR1 |= TIM_CR1_CEN;  // Counter Enable
}


//void Timer3_Init(void)
//{
//    // 1. Clock
//    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
//    
//    // 2. PSC:7200(得到10kHz频率)  ARR: 10000(1万次产生一个UEV) 
//    TIM3->PSC = 7200 -1;     //初次赋值
//    TIM3->ARR = 10000 -1;
//    
//    // 3. 打开更新中断使能位
//    TIM3->DIER |= TIM_DIER_UIE;
//    
//    // 4. NVIC配置
//    NVIC_SetPriorityGrouping(3);
//    NVIC_SetPriority(TIM3_IRQn, 2);
//    NVIC_EnableIRQ(TIM3_IRQn);
//    
//    // 5. 开启定时器
//    TIM3->CR1 |= TIM_CR1_CEN;
//}

//uint8_t isFirst = 0;

//void TIM3_IRQHandler(void)
//{
//    // 清除更新中断标志位
//    TIM3->SR &= ~ TIM_SR_UIF;
//    
//    if(isFirst == 0)
//    {
//        isFirst = 1;
//        return;
//    }
//    
//    LED_Toggle(LED[1]);
//}

// # Std库开发方式
//void Timer_Init(void)
//{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	
//	TIM_InternalClockConfig(TIM2);
//	
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
//	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
//	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
//	
//	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	TIM_Cmd(TIM2, ENABLE);
//}




