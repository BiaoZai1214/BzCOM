#include "App_bootloader.h"

/* 常量定义 */
#define RX_TIMEOUT_MS    2000   // 接收超时时间（毫秒）

/* 全局变量 */
uint8_t  app_rec_start_buff[64] = {0};
uint16_t app_rec_start_len = 0;
uint8_t  flag = 0;                     // KEY1 中断标志

extern uint32_t last_rec_time;         // 来自 bootloader.c
extern uint16_t uart_rec_full_len;      // 来自 bootloader.c

Bootloader_status boot_status = BOOTLOADER_STATUS_INIT;

/* 外部函数声明 */
void Int_bootloader_receive_app(void);
void Int_bootloader_erase_flash(uint32_t page_addr, uint16_t pages);
uint8_t Int_bootloader_jump_to_app(void);

/**
 * @brief KEY1 外部中断回调
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY1_Pin)
    {
        flag = 1;
    }
}

/**
 * @brief Bootloader 初始化
 */
void App_bootloader_init(void)
{
    printf("\r\n========== Bootloader ==========\r\n");
    printf("send 'start' to begin\r\n");
    boot_status = BOOTLOADER_STATUS_INIT;
}

/**
 * @brief 状态: INIT - 等待 start 命令
 */
static void State_Init(void)
{
    HAL_UARTEx_ReceiveToIdle(&huart2, app_rec_start_buff, 64,
                             &app_rec_start_len, 0xffffff);
    if (app_rec_start_len > 0)
    {
        if (strstr((char *)app_rec_start_buff, "start") != NULL)
        {
            printf("start received\r\n");
            boot_status = BOOTLOADER_STATUS_RUN;
        }
        memset(app_rec_start_buff, 0, sizeof(app_rec_start_buff));
        app_rec_start_len = 0;
    }
}

/**
 * @brief 状态: RUN - 准备接收（擦除 Flash，启动串口）
 */
static void State_Run(void)
{
    Int_bootloader_erase_flash(APP_START_ADDR, 10);
    printf("flash erased, ready to receive\r\n");

    uart_rec_full_len = 0;
    last_rec_time = 0;
    flag = 0;

    Int_bootloader_receive_app();
    boot_status = BOOTLOADER_STATUS_REC_DATA;
}

/**
 * @brief 状态: REC_DATA - 等待接收完成（KEY1 按下 或 超时）
 */
static void State_RecData(void)
{
    // KEY1 按下 -> 立即跳转
    if (flag)
    {
        flag = 0;
        printf("key1 pressed, len=%d\r\n", uart_rec_full_len);
        boot_status = BOOTLOADER_STATUS_JUMP_APP;
        return;
    }

    // 超时 -> 等待 2s 无数据则认为传输完成
    if (last_rec_time != 0 && (HAL_GetTick() - last_rec_time > RX_TIMEOUT_MS))
    {
        printf("rx timeout, len=%d\r\n", uart_rec_full_len);
        boot_status = BOOTLOADER_STATUS_JUMP_APP;
    }
}

/**
 * @brief 状态: JUMP_APP - 跳转到 APP
 */
static void State_JumpApp(void)
{
    printf("jumping to app...\r\n");
    if (Int_bootloader_jump_to_app() != 0)
    {
        printf("jump failed, try again\r\n");
        boot_status = BOOTLOADER_STATUS_INIT;
    }
}

/**
 * @brief 状态机主循环
 */
void App_bootloader_work(void)
{
    switch (boot_status)
    {
        case BOOTLOADER_STATUS_INIT:
            State_Init();
            break;

        case BOOTLOADER_STATUS_RUN:
            State_Run();
            break;

        case BOOTLOADER_STATUS_REC_DATA:
            State_RecData();
            break;

        case BOOTLOADER_STATUS_JUMP_APP:
            State_JumpApp();
            break;

        default:
            break;
    }
}
