# 串口接收调试笔记

## 错误1：缓冲区管理逻辑错误

### 问题描述
IAP 升级时，明明发送了 1900 字节，但只接收到 1644 字节，丢失了 256 字节（一整包）。

### 根本原因
`HAL_UARTEx_RxEventCallback()` 和 `uart_get_data()` **使用不同的缓冲区**：

```c
// 错误的原始实现
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 保存到 current_buff
    if (current_buff == buff1)
        memcpy(buff2, buff1, Size);  // 复制到 buff2
    else
        memcpy(buff1, buff2, Size);  // 复制到 buff1
}

uint8_t uart_get_data(uint8_t **buff, uint16_t *len)
{
    // 从 last_rx_buff 读取
    *buff = last_rx_buff;  // 完全不同的缓冲区！
}
```

### 修复方案
使用**双缓冲 + 固定读取缓冲区**：

```c
typedef struct {
    uint8_t *rx_buff;      // 当前接收缓冲区
    uint8_t buff1[256];
    uint8_t buff2[256];
    uint16_t rx_len;       // 当前包长度
} UART_DoubleBuffTypeDef;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 如果上一包还没被读取，丢弃新数据避免覆盖
    if (uart_double_buff.rx_len != 0) {
        HAL_UARTEx_ReceiveToIdle_IT(huart, uart_double_buff.rx_buff, BUFF_SIZE);
        return;
    }

    // 复制数据到固定读取缓冲区 buff1
    if (uart_double_buff.rx_buff == uart_double_buff.buff1)
        memcpy(uart_double_buff.buff2, uart_double_buff.buff1, Size);
    else
        memcpy(uart_double_buff.buff1, uart_double_buff.buff2, Size);

    uart_double_buff.rx_len = Size;
    last_rec_time = HAL_GetTick();

    // 切换接收缓冲区
    uart_double_buff.rx_buff = (uart_double_buff.rx_buff == buff1) ? buff2 : buff1;

    HAL_UARTEx_ReceiveToIdle_IT(huart, uart_double_buff.rx_buff, BUFF_SIZE);
}

uint8_t uart_get_data(uint8_t **buff, uint16_t *len)
{
    if (uart_double_buff.rx_len == 0) return 0;
    *buff = uart_double_buff.buff1;  // 始终从 buff1 读取
    *len = uart_double_buff.rx_len;
    uart_double_buff.rx_len = 0;
    return 1;
}
```

### 关键点
1. **上一包未处理时，丢弃新数据** - 避免覆盖
2. **始终从 buff1 读取** - 回调会复制到 buff1
3. **rx_len 作为"数据就绪"标志** - 非零表示有数据待处理

---

## 错误2：HAL库宏定义冲突

### 问题描述
编译警告：
```
warning: incompatible redefinition of macro "FLASH_BASE"
warning: incompatible redefinition of macro "FLASH_PAGE_SIZE"
```

### 根本原因
在 `bootloader.h` 中定义了与 CMSIS/HAL 库相同的宏：
```c
#define FLASH_BASE        0x08000000   // 与 stm32f103xb.h 冲突
#define FLASH_PAGE_SIZE   0x800        // 与 stm32f1xx_hal_flash_ex.h 冲突
```

### 修复方案
删除重复定义，使用 HAL 库提供的宏，或换用不同的名字：
```c
// 改用其他名字避免冲突
#define BOOT_ADDR         0x08000000
#define BOOT_SIZE         (16*1024)
```

---

## 错误3：消抖时间过短

### 问题描述
KEY1 按下后无反应

### 原因分析
按键消抖时间 30ms 太短，且高电平触发却写成检测低电平

### 修复
```c
// 检测高电平（按键按下为高）
if (pin == GPIO_PIN_SET && key_state == 1)
{
    if (HAL_GetTick() - key_press_tick > 50)  // 50ms 消抖
```

---

## 教训总结

1. **双缓冲设计**：一个缓冲区接收，一个缓冲区处理，避免竞争
2. **HAL库符号检查**：自定义宏前先搜索是否与 HAL/CMSIS 冲突
3. **调试输出**：`printf` 会增加延迟，可能导致数据丢失，调试完后删除
4. **IAP 超时设置**：空闲超时要大于发送间隔，本次设为 5 秒
