#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <string.h>
#include "Delay.h"
#include "led.h"
#include "KEY.h"
#include "usart.h"


// 定义你的APP起始地址
#define APP_START_ADDR 0x08004000   


    

// 然后在 main 函数里调用

int main()
{
    __disable_irq();	// 关闭中断
    SCB->VTOR = APP_START_ADDR;
    __enable_irq();		// 打开中断
	
    USART2_Init();
	LED_Init();
    
    while(1)
    {
		LED_Toggle(LED[2]);
		Delay_ms(500);
    }
    
}


