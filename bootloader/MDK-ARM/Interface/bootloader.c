#include "bootloader.h"
#include "at24c64.h"

/* UART IAP 外部变量 */
extern UART_DoubleBuffTypeDef uart_double_buff;
extern volatile uint8_t uart_recv_done;
extern volatile uint8_t uart_error_flag;
extern volatile uint32_t last_rec_time;

/*============================================================*/
/* 分区管理                                                   */
/*============================================================*/
uint32_t Boot_GetActiveBank(void)
{
    uint8_t flag = AT24C64_R_Byte(ADDR_ACTIVE_BANK);
    return (flag == FLAG_BANK_B) ? ADDR_APP_B : ADDR_APP_A;
}

void Boot_SetActiveBank(uint32_t addr)
{
    uint8_t flag = (addr == ADDR_APP_B) ? FLAG_BANK_B : FLAG_BANK_A;
    AT24C64_W_Byte(ADDR_ACTIVE_BANK, flag);
}

/*============================================================*/
/* Flash 操作                                                */
/*============================================================*/
void Boot_EraseFlash(uint32_t addr, uint16_t pages)
{
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks     = FLASH_BANK_1,
        .PageAddress = addr,
        .NbPages   = pages
    };
    uint32_t err;
    HAL_FLASHEx_Erase(&erase, &err);
    HAL_FLASH_Lock();
}

void Boot_CopyFlash(uint32_t src, uint32_t dst, uint32_t size)
{
    HAL_FLASH_Unlock();
    uint32_t pages = (size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks     = FLASH_BANK_1,
        .PageAddress = dst,
        .NbPages   = pages
    };
    uint32_t err;
    HAL_FLASHEx_Erase(&erase, &err);

    for (uint32_t i = 0; i < size; i += 2) {
        uint16_t data = *(volatile uint16_t *)(src + i);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, dst + i, data);
    }
    HAL_FLASH_Lock();
}

/*============================================================*/
/* UART IAP                                                  */
/*============================================================*/
void Boot_StartUartIap(void)
{
    /* 清空串口脏数据 */
    __HAL_UART_FLUSH_DRREGISTER(&huart1);
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    __HAL_UART_CLEAR_FEFLAG(&huart1);
    __HAL_UART_CLEAR_NEFLAG(&huart1);
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);

    uart_recv_done   = 0;
    uart_error_flag  = 0;
    last_rec_time    = HAL_GetTick();

    /* 初始化双缓冲 */
    uart_double_buff.rx_buff = uart_double_buff.buff1;
    uart_double_buff.rx_len  = 0;

    HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart_double_buff.rx_buff, BUFF_SIZE);
}

/*============================================================*/
/* 安全跳转                                                  */
/*============================================================*/
uint8_t Boot_JumpToApp(uint32_t app_addr)
{
    typedef void (*pFunc)(void);

    /* 校验栈顶地址 */
    uint32_t stack_ptr = *(volatile uint32_t *)(app_addr);
    if ((stack_ptr & SRAM_STACK_MASK) != SRAM_STACK_BASE) {
        return 1;
    }

    /* 校验复位向量 */
    uint32_t reset_vec = *(volatile uint32_t *)(app_addr + 4);
    if (reset_vec < app_addr || reset_vec >= ADDR_FLASH_END) {
        return 1;
    }

    /* 关闭外设 */
    NVIC_DisableIRQ(EXTI9_5_IRQn);
    NVIC_DisableIRQ(USART1_IRQn);
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    __disable_irq();

    /* 设置栈指针和中断向量表 */
    __set_MSP(stack_ptr);
    SCB->VTOR = app_addr;

    /* 跳转 */
    pFunc jump = (pFunc)reset_vec;
    jump();

    return 0;
}
