#include "Delay.h"
/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t us)
{
    // 1. 设置定时器重装值
	SysTick->LOAD = 72 * us;				
	// 2. 清空当前计数值
    SysTick->VAL = 0x00;					
	// 3. 设置时钟源为HCLK，启动定时器
    SysTick->CTRL = 0x00000005;
    // 4. 等待计数到0,判断CTRL的计数标志位是否为1
	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG) == 0);
    // 5. 关闭定时器	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE;				
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t ms)
{
	while(ms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t s)
{
	while(s--)
	{
		Delay_ms(1000);
	}
} 
