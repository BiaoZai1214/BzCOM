#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY_TIME_DOUBLE    0
#define KEY_TIME_LONG      1500
#define KEY_TIME_REPEAT    100

// 按键组
typedef enum {
    KEY1 = 0,
    KEY2,
    KEY_COUNT
} KeyId_t;

// 电平状态
typedef enum {
    LEVEL_UP = 0,
    LEVEL_DOWN
} KeyLevel_t;

// 事件标志
typedef enum {
    KEY_HOLD   = 0x01,
    KEY_DOWN   = 0x02,
    KEY_UP     = 0x04,
    KEY_SINGLE = 0x08,
    KEY_DOUBLE = 0x10,
    KEY_LONG   = 0x20,
    KEY_REPEAT = 0x40
} KeyEvent_t;

void KEY_Init(void);
uint8_t Key_Status(uint8_t key_id, uint8_t event);
void Key_Tick(void);

#endif
