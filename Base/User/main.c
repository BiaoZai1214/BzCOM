#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "Delay.h"


#define ADDR 0x08004000

	
int main()
{
	// 重定向中断向量表
	SCB->VTOR = ADDR;
	// 打开中断
	__enable_irq();
	
    LED_Init();

    while (1)
    {
        LED_Toggle(LED[1]);
        Delay_ms(500);
    }
}