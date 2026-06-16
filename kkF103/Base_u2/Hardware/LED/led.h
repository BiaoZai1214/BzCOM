#ifndef __LED_H
#define __LED_H

#include "Delay.h"

extern uint8_t LED_Mode;


/*  LED编号  */
typedef enum {
    LED1 = 0,
    LED2,
    LED3,
    LED_COUNT
}LEDId_t;

typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t      pin ;
    
} LEDPin_t;

extern const LEDPin_t LED[LED_COUNT];

typedef enum {
    OFF = 0,
    ON = 1
    
} LEDState_t;



void LED_Init(void);
// # 功能性代码
void LED_Set(LEDPin_t led, LEDState_t state);
void LED_Toggle(LEDPin_t led);
// # 项目式代码
void LED_Flash(LEDPin_t *leds, uint8_t count);

void LED_Tick(void);
#endif
