#include "myfreertos.h"
/* freertos 的相关头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* 其他头文件 */
#include "stdio.h"
#include "usart.h"

// # 启动任务的配置
#define START_TASK_STACK    128
#define START_TASK_PRIORITY 1
TaskHandle_t start_Task_Handle;
void Start_Task(void *pvParam);


// # 任务1的配置
#define TASK1_STACK    128
#define TASK1_PRIORITY 1
TaskHandle_t Task1_Handle;
void Task1(void *pvParam);

// # 任务2
#define TASK2_STACK    128
#define TASK2_PRIORITY 1
TaskHandle_t Task2_Handle;
void Task2(void *pvParam);

// # 任务3 
#define TASK3_STACK    128
#define TASK3_PRIORITY 1
TaskHandle_t Task3_Handle;                  
void Task3(void *pvParam);


void freertos_start(void)
{   
    // # 创建启动任务
    xTaskCreate(
        (TaskFunction_t)			Start_Task,				//任务函数地址
        (char *)					"Start_Task",			//任务名 字符串
        (configSTACK_DEPTH_TYPE)	START_TASK_STACK,      	//任务栈大小,默认最小128
        (void *)                  	NULL,             		//传递给任务的参数
        (UBaseType_t)            	START_TASK_PRIORITY,	//任务优先级
        (TaskHandle_t *)         	&start_Task_Handle      //任务句柄
    
    );	            
    vTaskStartScheduler();    
}

// # 启动任务：用来创建其他Task
static void Start_Task(void *pvParam)
{
    // # 进入临界区: 保护临界区内的代码不会被打断
    taskENTER_CRITICAL();
    
	// # 静态创建3个任务 
	xTaskCreate(
        (TaskFunction_t)          Task1,
        (char *)                 "Task1",	
        (configSTACK_DEPTH_TYPE)  TASK1_STACK,     
        (void *)                  NULL, 
        (UBaseType_t)             TASK1_PRIORITY,
        (TaskHandle_t *)         &Task1_Handle
        );
				
	xTaskCreate(
        (TaskFunction_t)          Task2,	
        (char *)                 "Task2",		
        (configSTACK_DEPTH_TYPE)  TASK2_STACK,     
        (void *)                  NULL,                 
        (UBaseType_t)             TASK2_PRIORITY,
        (TaskHandle_t *)         &Task2_Handle
        );
				
	xTaskCreate(
        (TaskFunction_t)          Task3,		
        (char *)                 "Task3",		
        (configSTACK_DEPTH_TYPE)  TASK3_STACK,     
        (void *)                  NULL,                 
        (UBaseType_t)             TASK3_PRIORITY,
        (TaskHandle_t *)         &Task3_Handle
        );
                      
    // # 退出临界区                   
    taskEXIT_CRITICAL();
	
	// 启动任务只需要执行一次,用完就删
	vTaskDelete(NULL);
}

void Task1(void *pvParam)
{
    for(;;){
        HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
        vTaskDelay(1000);
    }
}

void Task2(void *pvParam)
{
    for(;;){
		HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
        vTaskDelay(2000);
    }
}

void Task3(void *pvParam)
{
     for(;;){
		HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin);
        vTaskDelay(3000);
    }
}
