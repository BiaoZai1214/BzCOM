#ifndef __KEY_H
#define __KEY_H

/* 引用头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "usart.h"


/* 宏定义 */
#define READ_KEY1()    HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define READ_KEY2()    HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)

/* 定义变量 */
typedef enum {
    Key_None = 0,
    Key1_Pressed = 1,
    Key2_Pressed = 2
} KeyEvent_t;

// 函数声明
KeyEvent_t Key_Scan(void);

#endif


