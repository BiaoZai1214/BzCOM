#include "Key.h"


KeyEvent_t Key_Scan(void)
{
    KeyEvent_t key = Key_None;

    if (READ_KEY1() == GPIO_PIN_RESET || READ_KEY2() == GPIO_PIN_RESET) 
    { 
        HAL_Delay(20);
        if(READ_KEY1() == GPIO_PIN_RESET) key = Key1_Pressed;
        if(READ_KEY2() == GPIO_PIN_RESET) key = Key2_Pressed;
    }
    return key;
}
