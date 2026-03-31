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

void Boot_WriteFlash(uint32_t addr, const uint8_t *data, uint32_t len)
{
    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i + 1 < len; i += 2) {
        uint16_t halfword = data[i] | ((uint16_t)data[i + 1] << 8);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, halfword);
    }
    /* 奇数长度：补 0 写入最后一个半字 */
    if (len & 1) {
        uint16_t halfword = data[len - 1];
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + len - 1, halfword);
    }
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

    uint8_t buff[256];
    for (uint32_t off = 0; off < size; off += sizeof(buff)) {
        uint32_t chunk = (size - off >= sizeof(buff)) ? sizeof(buff) : (size - off);
        memcpy(buff, (const uint8_t *)(src + off), chunk);
        Boot_WriteFlash(dst + off, buff, chunk);
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
/* 固件校验                                                  */
/*============================================================*/
uint8_t Boot_IsValidFirmware(uint32_t addr)
{
    uint32_t stack = *(volatile uint32_t *)addr;
    return (stack >= SRAM_STACK_BASE && stack <= SRAM_MAX);
}

/*============================================================*/
/* UART 辅助                                                 */
/*============================================================*/
void UART_ClearError(void)
{
    uart_error_flag = 0;
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
