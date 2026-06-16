#include "KEY.h"
#include "stm32f10x.h"                  // Device header

// # 构造一个uint8_t数组,容纳事件状态(7种),初始化0
volatile uint8_t Key_Flag[KEY_COUNT] = {0}; 

// # 构建一个结构体列表给KeyPin_t类型初始化
static const KeyPin_t KeyList[KEY_COUNT] = {
    [KEY1] = {GPIOB, (1U << 12)},
    [KEY2] = {GPIOB, (1U << 13)}
};

// # 按键初始化
void KEY_Init(void)
{
    // # 1. GPIO -> AFIO 时钟使能: 打开引脚复用
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
//    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    
    // # 2 GPIO工作模式 PB12 PB13 CNF: 10 MODE: 00
    GPIOB->CRH &= ~(0xff << (4 * 4));
    GPIOB->CRH |=  (0x08 << (4 * 4));
    
    GPIOB->CRH &= ~(0xff << (5 * 4));
    GPIOB->CRH |=  (0x08 << (5 * 4));
    
    /* 给ODR高电平 == 配置内部上拉 */
    GPIOB->ODR |= GPIO_ODR_ODR12 | GPIO_ODR_ODR13;
    
    // # 下面是外部中断的配置
//    // # 3 AFIO_EXTI引脚复用选择: 分为4个组0:[0-3] [4-7] [8-11] [12-15]
//    AFIO->EXTICR[3] &= ~0xFF;
//    AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI12_PB | AFIO_EXTICR4_EXTI13_PB;
//    
//    // # 4 配置EXTI
//    EXTI->RTSR |= EXTI_FTSR_TR12;       // 下降沿检测
//    EXTI->IMR  |= EXTI_IMR_MR12;        // 中断屏蔽寄存器
//    
//    EXTI->RTSR |= EXTI_FTSR_TR13;       // 下降沿检测
//    EXTI->IMR  |= EXTI_IMR_MR13;        // 中断屏蔽寄存器
//    
//    // # SWIRE (软件中断寄存器)...
//    // 此函数直接检测GPIO电平,不需要软件控制进入
//    
//    // # 5 NVIC分组配置(NVIC相当于在内核,所以直接用cm3定义库的函数)
//    NVIC_SetPriorityGrouping(2);
//    NVIC_SetPriority(EXTI15_10_IRQn, 2); // 
//    
//    NVIC_EnableIRQ(EXTI15_10_IRQn);      // 
}

// 获取按键类型: 按下 OR 没按
uint8_t Key_GetState(KeyPin_t key)
{
    // 读取传入的 按键端口 & 对应位 的电平信号是否为0
    if((key.port->IDR & key.pin) == 0)
    {
        return KEY_PRESSED;
    }
    return KEY_UNPRESSED;
    
}

// 主函数接口: 比对传参Flag 看 Key_Flag[]有没有被别的状态赋值
uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
	if (Key_Flag[n] & Flag)     //状态标志位变量 & 传参
	{
		if (Flag != KEY_HOLD)
		{
			Key_Flag[n] &= ~Flag; // 除了HOLD位保持,其他位检测即清0
		}
		return 1;
	}
	return 0;
}

/*
*       按键状态机
*       分解按键的三个状态: 空闲 按下 按住
*       用三个基础状态,加上等待时间,整合出7种事件
*/

void Key_Tick(void)
{
	static uint8_t Count, i;
    // #           当前状态              上次状态
	static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];
	static uint8_t State[KEY_COUNT];
	static uint16_t Time[KEY_COUNT];
	
	for (i = 0; i < KEY_COUNT; i ++)
	{
		if (Time[i] > 0)
		{
			Time[i] --;
		}
	}
	
	Count ++;
	if (Count >= 20)    // 延时20ms消抖
	{
		Count = 0;
        
		for (i = 0; i < KEY_COUNT; i ++)
		{
			//PrevState[i] = CurrState[i];        // -1<0<1
			CurrState[i] = Key_GetState(KeyList[i]);
			
            // 按键的三种状态 空闲 按下 按住...(此处省略,放在注释)

            switch (State[i])
            {
                case 0: //  按键已按下
                    if (CurrState[i] == KEY_PRESSED)
                    {
                        Time[i] = KEY_TIME_LONG;
                        State[i] = 1;
                    }
                    break;

                case 1: //  按键已松开
                    if (CurrState[i] == KEY_UNPRESSED)
                    {
                        Time[i] = KEY_TIME_DOUBLE;    // 双击等待时间
                        State[i] = 2;
                    }   
                    // 时间耗尽仍按住: 进入长按
                    else if (Time[i] == 0)
                    {
                        Time[i] = KEY_TIME_REPEAT;
                        Key_Flag[i] |= KEY_LONG;
                        State[i] = 4;
                    }
                    break;

                case 2: //  按键已双击
                    if (CurrState[i] == KEY_PRESSED)
                    {
                        Key_Flag[i] |= KEY_DOUBLE;
                        State[i] = 3;
                    }
                    // 双击时间内没再次按下: 定为单击
                    else if (Time[i] == 0)
                    {
                        Key_Flag[i] |= KEY_SINGLE;
                        State[i] = 0;
                    }
                    break;

                case 3: //  按键已松开: 双击完成返回空闲
                    if (CurrState[i] == KEY_UNPRESSED)
                    {
                        State[i] = 0;
                    }
                    break;

                case 4: //  按键已长按: 松开返回空闲
                    if (CurrState[i] == KEY_UNPRESSED)
                    {
                        State[i] = 0;
                    }
                    else if (Time[i] == 0)
                    {
                        Time[i] = KEY_TIME_REPEAT;
                        Key_Flag[i] |= KEY_REPEAT;
                        // State[i] = 4; // 可省略，因为本来就是 4
                    }
                    break;

                default:
                    // 异常状态:返回到空闲状态
                    State[i] = 0;
                    break;
            }
		}
	}
}

//// 按键的三种状态 空闲 按下 按住...(放在后面注释)
//if (CurrState[i] == KEY_PRESSED)
//{
//	Key_Flag[i] |= KEY_HOLD;
//}
//else
//{
//	Key_Flag[i] &= ~KEY_HOLD;
//}

//if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
//{
//	Key_Flag[i] |= KEY_DOWN;
//}

//if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
//{
//	Key_Flag[i] |= KEY_UP;
//}

/*      按键回调函数      */
//// # 1. 定义一个函数指针类型 の 函数指针 (初始化为空);
//static KEY_Callback_t key_callback = NULL;

//// # 2.构造一个注册函数,把 用户函数 の 地址 -赋值-> 到 函数指针
//void Key_RegCallback(KEY_Callback_t callback)
//{
//    key_callback = callback;
//}

//void EXTI15_10_IRQHandler(void)
//{   
//    // 是否由PB12触发了中断回调
//    if((EXTI->PR & EXTI_PR_PR12) != 0)
//    {
//        // # 清除中断挂起标志位
//        EXTI->PR = EXTI_PR_PR12;
//        
//        // # 当回调函数被调用时
//        if (key_callback != NULL)
//        {
//            key_callback();
//        }
//    }
//    // 是否由PB13触发了中断回调
//    if((EXTI->PR & EXTI_PR_PR13) != 0)
//    {
//        // # 清除中断挂起标志位
//        EXTI->PR = EXTI_PR_PR13;
//        
//        // # 当回调函数被调用时
//        if (key_callback != NULL)
//        {
//            key_callback();
//        }
//    }
//}



