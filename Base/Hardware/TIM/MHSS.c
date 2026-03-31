#include "MHSS.h"


void TIM2_ETR_Init(void)
{
    // 1. 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // TIM2

    // 2. PA0 配置为上拉输入（更稳定！）
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= GPIO_CRL_CNF0_1;        // CNF=10, MODE=00 → 上拉/下拉
    GPIOA->ODR |= GPIO_ODR_ODR0;          // 启用上拉（关键！）

    // 3. 配置 TIM2
    TIM2->CR1 = 0;
    TIM2->SMCR = 0;

    // 3.1 降低 fDTS → 增大滤波时间窗口
    TIM2->CR1 |= TIM_CR1_CKD;  // CKD = 10 → fDTS = fCK_INT / 4 (e.g., 72/4=18MHz)

    // 3.2 最大滤波 + 预分频
    TIM2->SMCR |= TIM_SMCR_ETF;
    TIM2->SMCR &= ~TIM_SMCR_ETPS;

    // 3.3 上升沿触发
    TIM2->SMCR &= ~TIM_SMCR_ETP;

    // 3.4 启用外部时钟模式2
    TIM2->SMCR |= TIM_SMCR_ECE;

    // 3.5 自动重载 + 启动
    TIM2->ARR = 0xFFFF;
    TIM2->CR1 |= TIM_CR1_CEN;
}

