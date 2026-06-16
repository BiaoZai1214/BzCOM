#include "bootloader.h"


// 接收程序的缓冲区
uint8_t uart_rec_buff[BOOTLOADER_UART_REC_BUFF_LEN] = {0};
uint16_t uart_rec_len = 0;              // 本次接收的字节数
uint16_t uart_rec_full_len = 0;         // 累计接收的字节数

// Flash 写入偏移量
uint32_t flash_write_offset = 0;
// 上次接收时间（用于超时检测）
uint32_t last_rec_time = 0;

// 奇偶对齐处理：遗留的单字节
// 由于 Flash 必须按半字（2字节）写入，当总字节数为奇数时，最后一个字节需要缓存到下次
uint8_t last_byte_flag = 0;  // 是否有遗留字节
uint8_t last_byte = 0;       // 遗留的单个字节

/**
 * @brief 将接收缓冲区的数据写入 Flash（半字对齐）
 *
 * 处理逻辑：
 *   1. 如果有遗留字节，先与当前第一个字节配对写入
 *   2. 剩余数据成对写入（每两个字节组成一个半字）
 *   3. 如果最后剩单个字节，缓存到下次处理
 *
 * 示例：
 *   场景A: 上次遗留无，本次[0x11,0x22,0x33] → 写入0x2211，遗留0x33
 *   场景B: 上次遗留0x33，本次[0x44,0x55]    → 写入0x4433，剩余0x5544待写入
 */
static void Int_flash_write_halfword(void)
{
    uint16_t i = 0;

    // Step 1: 处理遗留字节
    // 上次如果收了奇数字节，会有一个字节没配对，先把它跟本次第一个字节组合
    if (last_byte_flag)
    {
        uint32_t addr = APP_START_ADDR + flash_write_offset;
        uint16_t data = last_byte | (uart_rec_buff[0] << 8);  // 低字节=遗留字节，高字节=新字节
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, data);
        flash_write_offset += 2;
        i = 1;  // 从第二个字节开始处理
        last_byte_flag = 0;
    }

    // Step 2: 批量写入完整半字
    // 每次处理两个相邻字节: [byte[i], byte[i+1]] → 半字(byte[i+1]<<8 | byte[i])
    for (; i + 1 < uart_rec_len; i += 2)
    {
        uint32_t addr = APP_START_ADDR + flash_write_offset;
        uint16_t data = uart_rec_buff[i] | (uart_rec_buff[i + 1] << 8);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, data);
        flash_write_offset += 2;
    }

    // Step 3: 处理剩余单字节
    // 如果处理完成对字节后还剩一个，缓存到下次
    if (i < uart_rec_len)  // i == uart_rec_len - 1，只剩最后一个字节
    {
        last_byte = uart_rec_buff[i];
        last_byte_flag = 1;
    }
}

//
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        // 接收到数据 记录当前的STM32系统时间  单位ms
        last_rec_time = HAL_GetTick();

        // 保存实际接收的数据长度
        uart_rec_len = Size;
        uart_rec_full_len += uart_rec_len;

        // 将接收的数据写入到flash
        // 1. 解锁flash
        HAL_FLASH_Unlock();

        // 2. 使用16位写入
        // flash写入一次16位  40us*256 = 10ms
        Int_flash_write_halfword();

        // 3. 重新加锁
        HAL_FLASH_Lock();

        // 使用完数据之后  清空 准备下一次的接收
        memset(uart_rec_buff, 0, BOOTLOADER_UART_REC_BUFF_LEN);
        // 清空掉初始化串口使用之前的所有问题
        __HAL_UART_CLEAR_OREFLAG(&huart2);
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        HAL_UARTEx_ReceiveToIdle_IT(&huart2, uart_rec_buff, BOOTLOADER_UART_REC_BUFF_LEN);
    }
}

/**
 * @brief 串口接收 => 准备接收A程序
 *
 */
void Int_bootloader_receive_app(void)
{
    // 清空掉初始化串口使用之前的所有问题
    __HAL_UART_CLEAR_OREFLAG(&huart2);
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);
    // 带有中断的串口接收函数
    // 少一个参数 => 超时时间  因为IT带中断的函数方法是异步执行的
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, uart_rec_buff, BOOTLOADER_UART_REC_BUFF_LEN);
}

/**
 * @brief 跳转到APP程序
 * @return 0:成功 1:失败
 */
uint8_t Int_bootloader_jump_to_app(void)
{
    typedef void (*pFunc)(void);

    // 1. 校验APP有效性
    uint32_t app_stack_ptr = *(volatile uint32_t *)(APP_START_ADDR);
    uint32_t app_reset_handle = *(volatile uint32_t *)(APP_START_ADDR + 4);

    // 1.1 校验栈顶地址（高16位必须是0x2000，即SRAM区域）
    if ((app_stack_ptr & 0xFFFF0000) != STACK_ADDR)
    {
        printf("stack addr error: 0x%08X\r\n", app_stack_ptr);
        return 1;
    }

    // 1.2 校验复位向量地址必须在APP区域内
    if (app_reset_handle < APP_START_ADDR || app_reset_handle > APP_END_ADDR)
    {
        printf("reset handler error: 0x%08X\r\n", app_reset_handle);
        return 1;
    }

    printf("app valid, jumping...\r\n");

    // 2. 准备跳转 - 关闭所有外设和中断
    __disable_irq();
    HAL_DeInit();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // 3. 设置栈顶和中断向量表
    __set_MSP(app_stack_ptr);
    SCB->VTOR = APP_START_ADDR;

    // 4. 跳转到APP
    pFunc jump_to_app = (pFunc)app_reset_handle;
    jump_to_app();

    // 不应到达此处
    return 1;
}

/**
 * @brief 外部可调用 提前擦除flash空间
 *
 * @param page_addr
 * @param pages
 */
void Int_bootloader_erase_flash(uint32_t page_addr, uint16_t pages)
{
    // 解锁flash
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase_init;
    // 擦除单独页
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    // 擦除第1个bank的页
    erase_init.Banks = FLASH_BANK_1;
    // 擦除页的起始地址
    erase_init.PageAddress = page_addr;
    // 擦除几页
    erase_init.NbPages = pages;
    uint32_t page_error = 0;
    // flash擦除比较耗费性能
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    // 加锁flash
    HAL_FLASH_Lock();
}
