#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

extern uint8_t LED_Mode;

typedef enum {
    LED1 = 0,
    LED2,
    LED3,
    LED_COUNT
} LEDId_t;

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} LEDPin_t;

extern const LEDPin_t LED[LED_COUNT];

typedef enum {
    OFF = 0,
    ON  = 1
} LEDState_t;

void LED_Init(void);
void LED_Set(LEDPin_t led, LEDState_t state);
void LED_Toggle(LEDPin_t led);
void LED_Tick(void);

#endif
