#ifndef __KEY_H_
#define __KEY_H_

#include "LED.h"
#include "Delay.h"
#include "stddef.h"

#define KEY_TIME_DOUBLE			0
#define KEY_TIME_LONG			1500
#define KEY_TIME_REPEAT			100

// 端口+电平信号 类型
typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t      pin;
    
}KeyPin_t;


/*  按键编号  */
typedef enum {
    KEY1 = 0,
    KEY2,
    KEY3,
    KEY_COUNT 
}KeyId_t;

/*  按键状态  没按 OR 按下 */
typedef enum {
    KEY_UNPRESSED = 0,
    KEY_PRESSED ,
}KeyType_t;

/*  按键事件  uint8_t的每一位设置一个事件*/
typedef enum {
    KEY_HOLD   =  0x01,
    KEY_DOWN   = (0x01 << 1),   
    KEY_UP     = (0x01 << 2), 
    KEY_SINGLE = (0x01 << 3), 
    KEY_DOUBLE = (0x01 << 4), 
    KEY_LONG   = (0x01 << 5),	
    KEY_REPEAT = (0x01 << 6)
}KeyState_t;

// # 构造一个函数指针
typedef void (*KEY_Callback_t)(void);

// # 构造一个注册函数,把 用户函数 の 地址 -赋值-> 到 函数指针
void Key_RegCallback(KEY_Callback_t callback);


void KEY_Init(void);
uint8_t KEY_GetNum(void);
uint8_t Key_GetState(KeyPin_t key);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Tick(void);



#endif

