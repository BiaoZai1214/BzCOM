#include "led.h"
#include "stm32f10x.h"                  // Device header

const LEDPin_t LED[LED_COUNT] = {
    [LED1] = {GPIOA, (1U << 6)},
    [LED2] = {GPIOA, (1U << 7)},
    [LED3] = {GPIOB, (1U << 0)}
};

uint8_t LED_Mode = 0;   // 外部可设置模式

void LED_Init(void)
{
    // #1 初始化 时钟使能
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // #2 初始化GPIOA_Pin6: 推挽输出 + 2Mhz
    
    /* 4位1组 用掩码0F取反: 一组0 << (组码 * 位数) 只对目标组清0 */
    GPIOA->CRL &= ~(0xFUL << (6 * 4));  
    GPIOA->CRL &= ~(0xFUL << (7 * 4));
    GPIOB->CRL &= ~(0xFUL << (0 * 4));
    
    /* 设置目标组的值 (推挽)00 + (2Mhz)10 */
    GPIOA->CRL |=  (0x02 << (6 * 4));    
    GPIOA->CRL |=  (0x02 << (7 * 4));
    GPIOB->CRL |=  (0x02 << (0 * 4));
    
    // 设置默认低电平
    GPIOA->BSRR = ((1U << 6) << 16) | ((1U << 7) << 16);  
    GPIOB->BSRR = GPIO_BSRR_BR0;
	
}

// 写LED的控制电平
void LED_Set(LEDPin_t led, LEDState_t state)
{
    led.port->BSRR = state ? led.pin : (led.pin << 16);
}

void LED_Toggle(LEDPin_t led)
{
    led.port->ODR ^= led.pin;
}

// # LED的状态机
void LED_Tick(void)
{
    static uint16_t tick = 0;
    tick++; 

    switch (LED_Mode)
    {
        case 0: // 全灭
            for (uint8_t i = 0; i < LED_COUNT; i++) {
                LED_Set(LED[i], OFF);
            }
            break;

        case 1: // 全亮
            for (uint8_t i = 0; i < LED_COUNT; i++) {
                LED_Set(LED[i], ON);
            }
            break;

        case 2: // 慢闪（1Hz）
        {
            uint8_t on = (tick % 1000) < 500;
            for (uint8_t i = 0; i < LED_COUNT; i++) {
                LED_Set(LED[i], on ? ON : OFF);
            }
            break;
        }

        case 3: // 快闪（10Hz）
        {
            uint8_t on = (tick % 100) < 50;
            for (uint8_t i = 0; i < LED_COUNT; i++) {
                LED_Set(LED[i], on ? ON : OFF);
            }
            break;
        }

        // case 4: 可用于呼吸灯、跑马灯等，此处省略
        default:
            break;
    }
}

//void TIM2_IRQHandler(void)
//{
//    // 检查是否是更新中断（UIF = 1）
//    if (TIM2->SR & TIM_SR_UIF)
//    {
//        // 清除更新中断标志（写 0 到 UIF 位）
//        TIM2->SR &= ~TIM_SR_UIF; 
//        
//        // 1ms 定时中断：轮询状态机
//        //Key_Tick();
//        LED_Tick();
//    }
//}