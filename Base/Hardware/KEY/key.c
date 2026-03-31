#include "KEY.h"

volatile uint8_t Key_Flag[KEY_COUNT] = {0};

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} Key_T;

static const Key_T Key_List[KEY_COUNT] = {
    {GPIOA, 1U <<  0},
    {GPIOC, 1U << 13},
};

typedef enum {
    IDLE = 0,
    PRESS,
    WAIT,
    DOUBLE,
    LONG
} State_t;

void KEY_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // PA0 浮空输入
    GPIOA->CRL &= ~(0xFUL  << 0);
    GPIOA->CRL |=  (0x04UL << 0);

    // PC13 浮空输入
    GPIOC->CRH &= ~(0xFUL  << (5*4));
    GPIOC->CRH |=  (0x04UL << (5*4));

    // 关闭上拉
    GPIOA->ODR &= ~(1U << 0);
    GPIOC->ODR &= ~(1U << 13);
}

static uint8_t Key_Read(uint8_t i)
{
    uint32_t idr = Key_List[i].port->IDR;
    return ((idr & Key_List[i].pin) == 0) ? LEVEL_DOWN : LEVEL_UP;
}

uint8_t Key_Status(uint8_t key_id, uint8_t event)
{
    if (Key_Flag[key_id] & event) {
        if (event != KEY_HOLD)
            Key_Flag[key_id] &= ~event;
        return 1;
    }
    return 0;
}

void Key_Tick(void)
{
    static uint8_t  scan;
    static uint8_t  curr[KEY_COUNT];
    static uint8_t  stat[KEY_COUNT];
    static uint16_t time[KEY_COUNT];

    uint8_t i;

    for (i=0; i<KEY_COUNT; i++)
        if (time[i]) time[i]--;

    if (++scan >= 20)
    {
        scan = 0;

        for (i=0; i<KEY_COUNT; i++)
        {
            curr[i] = Key_Read(i);

            switch (stat[i])
            {
                case IDLE:
                    if (curr[i] == LEVEL_DOWN) {
                        time[i] = KEY_TIME_LONG;
                        stat[i] = PRESS;
                    }
                    break;

                case PRESS:
                    if (curr[i] == LEVEL_UP) {
                        time[i] = KEY_TIME_DOUBLE;
                        stat[i] = WAIT;
                    } else if (time[i] == 0) {
                        time[i] = KEY_TIME_REPEAT;
                        Key_Flag[i] |= KEY_LONG;
                        stat[i] = LONG;
                    }
                    break;

                case WAIT:
                    if (curr[i] == LEVEL_DOWN) {
                        Key_Flag[i] |= KEY_DOUBLE;
                        stat[i] = DOUBLE;
                    } else if (time[i] == 0) {
                        Key_Flag[i] |= KEY_SINGLE;
                        stat[i] = IDLE;
                    }
                    break;

                case DOUBLE:
                    if (curr[i] == LEVEL_UP)
                        stat[i] = IDLE;
                    break;

                case LONG:
                    if (curr[i] == LEVEL_UP)
                        stat[i] = IDLE;
                    else if (time[i] == 0) {
                        time[i] = KEY_TIME_REPEAT;
                        Key_Flag[i] |= KEY_REPEAT;
                    }
                    break;

                default:
                    stat[i] = IDLE;
                    break;
            }
        }
    }
}
