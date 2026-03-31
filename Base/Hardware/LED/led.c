#include "led.h"

const LEDPin_t LED[LED_COUNT] = {
    {GPIOA, 1U << 1},
    {GPIOA, 1U << 2},
    {GPIOA, 1U << 3}
};

uint8_t LED_Mode = 0;

// 共阳极 LED 初始化
void LED_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA1 推挽输出 2MHz
    GPIOA->CRL &= ~(0xFUL  << (1 * 4));
    GPIOA->CRL |=  (0x02UL << (1 * 4));

    // PA2
    GPIOA->CRL &= ~(0xFUL  << (2 * 4));
    GPIOA->CRL |=  (0x02UL << (2 * 4));

    // PA3
    GPIOA->CRL &= ~(0xFUL  << (3 * 4));
    GPIOA->CRL |=  (0x02UL << (3 * 4));

    // 默认全灭
    for (uint8_t i = 0; i < LED_COUNT; i++)
        LED_Set(LED[i], OFF);
}

// 共阳极：ON = 低电平, OFF = 高电平
void LED_Set(LEDPin_t led, LEDState_t state)
{
    led.port->BSRR = state ? (led.pin << 16) : led.pin;
}

void LED_Toggle(LEDPin_t led)
{
    led.port->ODR ^= led.pin;
}

void LED_Tick(void)
{
    static uint16_t tick = 0;
    tick++;

    switch (LED_Mode)
    {
        case 0: // 全灭
            for (uint8_t i = 0; i < LED_COUNT; i++)
                LED_Set(LED[i], OFF);
            break;

        case 1: // 全亮
            for (uint8_t i = 0; i < LED_COUNT; i++)
                LED_Set(LED[i], ON);
            break;

        case 2: // 慢闪 1Hz
        {
            uint8_t on = (tick % 1000) < 500;
            for (uint8_t i = 0; i < LED_COUNT; i++)
                LED_Set(LED[i], on ? ON : OFF);
            break;
        }

        case 3: // 快闪 10Hz
        {
            uint8_t on = (tick % 100) < 50;
            for (uint8_t i = 0; i < LED_COUNT; i++)
                LED_Set(LED[i], on ? ON : OFF);
            break;
        }

        default:
            break;
    }
}
