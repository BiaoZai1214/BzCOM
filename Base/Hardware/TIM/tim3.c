#include "TIM3.h"


// 用输出比较(OC寄存器)输出一个PWM方波信号
// 一个方波周期内,高电平所占的比例: 占空比
// 所以说设置占空比也就是设置这个高电平的输出时间
void TIM3_Init(void)
{
    // 1. Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
    // 2. GPIO_Mode PA6: 复用推挽(CNF-10 MODE-11)
    GPIOA->CRL |=  GPIO_CRL_MODE6;
    GPIOA->CRL |=  GPIO_CRL_CNF6_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
    
    // 3. Timer
    // 3.1 PSC: 7200 - 1 (可以设置小一点,得到的频率越高)
    TIM3->PSC = 7200 - 1;
    
    // 3.2 ARR: 100 - 1
    TIM3->ARR = 100 - 1;
    
    // 3.3 计数方向(默认向上计数,不设置也没关系)
    TIM3->CR1 &= ~TIM_CR1_DIR;
    
    // 3.4 设置通道1 の CCR值
    TIM3->CCR1 = 50;
    
    // 3.5 配值通道1为输出模式(CC1S: 00)
    TIM3->CCMR1 &= ~TIM_CCMR1_CC1S;
    
    // 3.6 配置通道1为 PWM模式(OC1M:110)
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_2;
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1;
    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_0;
    
    // 3.7 使能输出通道(CCxE: x为使能通道位)
    TIM3->CCER |= TIM_CCER_CC1E;
    // 默认高电平有效
}

// 定时的开关包装成函数
void TIM3_Start(void)
{
    TIM3->CR1 |= TIM_CR1_CEN;
}
void TIM3_Stop(void)
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
}

// 设置占空比的函数
void TIM3_SetDutyCycle(uint8_t dutyCycle)
{
    TIM3->CCR1 = dutyCycle;
}

//  // 输出比较PWM信号的设置
//    TIM3_Start();   
//    
//    uint8_t dutyCycle = 0;
//    uint8_t dir = 0;
//    
//    
//	while(1)
//    {   
//        if(dir == 0)
//        {
//            // 占空比增大
//            dutyCycle ++;
//            if(dutyCycle >= 99)
//            {
//                dir = 1;
//            }
//            
//        }
//        else 
//        {
//            // 占空比减小
//            dutyCycle --;
//            if(dutyCycle <= 1)
//            {
//                dir = 0;
//            }
//            
//        }
//        
//        // 设置占空比
//        TIM3_SetDutyCycle(dutyCycle);
//        Delay_ms(20);
//    }
