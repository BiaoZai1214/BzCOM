# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

基于 **Keil uVision** 的 STM32F103C8T6 UART bootloader，通过串口接收并烧录 APP 程序到 Flash。

- **芯片**: STM32F103C8T6 (Cortex-M3, 64KB Flash, 20KB SRAM)
- **编译器**: ARM ADS 5.06 update 7 (build 960)
- **HAL库**: STM32Cube HAL F1 V1.8.7
- **输出**: `uart_bootloader.hex` / `uart_bootloader.bin`

## 目录结构

```
MDK-ARM/
├── 1.bootloader.uvprojx     # Keil 工程文件
├── 1.bootloader.uvoptx      # Keil 选项文件
├── startup_stm32f103xb.s   # 启动文件
├── Interface/               # Bootloader 核心逻辑
│   ├── bootloader.c/h       # Flash 读写、跳转、安全校验
│   └── (业务代码，不受 CubeMX 管理)
├── Application/             # 应用层状态机
│   └── App_bootloader.c/h  # bootloader 工作流程状态机
└── 1.bootloader/           # 构建输出目录
```

> **注意**: `Core/` 目录由 STM32CubeMX 管理，编辑 `1.bootloader.ioc` 后会重新生成。业务逻辑应写在 `MDK-ARM/Interface/` 和 `MDK-ARM/Application/` 中。

## Flash 布局

| 区域 | 地址范围 | 大小 |
|------|----------|------|
| Bootloader | 0x08000000 - 0x08003FFF | 16KB |
| APP | 0x08004000 - 0x0800FFFF | 48KB |

## 编译构建

使用 Keil uVision 打开 `MDK-ARM/1.bootloader.uvprojx`，按 **F7** 或点击 **Build**。

构建后自动调用 `fromelf.exe` 生成 `.bin` 文件到 `MDK-ARM/1.bootloader/` 目录。

## Bootloader 状态机工作流程

1. `BOOTLOADER_STATUS_INIT` - 等待用户发送 `start:len` 命令
2. `BOOTLOADER_STATUS_RUN` - 擦除 Flash，准备接收
3. `BOOTLOADER_STATUS_RX_DATA` - 接收数据（超时 `RX_TIMEOUT`=2s 认为接收完成）
4. `BOOTLOADER_STATUS_CHECK_DATA` - 校验长度
5. `BOOTLOADER_STATUS_JUMP_APP` - 跳转到 APP

## 关键模块

### bootloader.c (Interface/)

| 函数 | 说明 |
|------|------|
| `BootLoader_Init()` | 初始化串口空闲中断接收 |
| `Bootloader_EraseFlash(page_addr, pages)` | 擦除 Flash 页 |
| `Flash_Write_Halfword(data)` | 写入半字到 Flash |
| `Flash_Write_Packet()` | 写入一包数据到 Flash（处理奇偶对齐） |
| `Bootloader_JumpToApp()` | 安全跳转到 APP（校验栈顶和入口地址） |
| `Bootloader_GetStatus()` | 获取当前状态 |
| `Bootloader_SetStatus(status)` | 设置状态 |

### App_bootloader.c (Application/)

| 函数 | 说明 |
|------|------|
| `APP_Bootloader_Init()` | 打印欢迎信息，初始化状态为 INIT |
| `App_Bootloader_Work()` | 状态机主循环 |
| `APP_Bootloader_StartRx()` | 擦除 Flash 并切换到 RX_DATA 状态 |

### main.c (Core/Src/)

- 调用 `APP_Bootloader_Init()` 初始化
- 主循环调用 `App_Bootloader_Work()` 运行状态机

### HAL_UARTEx_RxEventCallback

- 串口空闲中断回调，标记 `rx_data_ready`
- 在中断内只做标记，不操作 Flash

## 全局变量 (bootloader.c)

```c
extern uint8_t   uart_rx_buffer[UART_RX_BUFF_LEN];  // 512B 接收缓冲
extern volatile uint16_t  uart_rx_len;               // 本次接收长度
extern volatile uint16_t  uart_rx_full_len;          // 累计接收长度
extern volatile uint8_t   rx_data_ready;             // 接收完成标记
extern volatile uint32_t last_rx_time;               // 上次接收时间（用于超时检测）
extern volatile uint32_t flash_write_offset;         // Flash 写入偏移
```

## 用户协议

1. 发送 `start:len` 开始传输（例如 `start:1024`）
2. 等待接收 `len` 字节的 APP 数据
3. 传输完成自动跳转 APP

## 硬件配置

| 外设 | 引脚 | 配置 |
|------|------|------|
| USART2 | PA2=TX, PA3=RX | 115200bps, 8N1, 空闲中断接收 |
| LED_B | PA6 | 输出 (低电平点亮) |
| LED_G | PA7 | 输出 (低电平点亮) |
| LED_R | PB0 | 输出 (低电平点亮) |
| KEY1 | PB13 | 输入 |
| KEY2 | PB12 | 输入 |

**系统时钟**: HSE(8MHz) → PLL × 9 = 72MHz

## 中断配置

- NVIC_PRIORITYGROUP_4
- USART2_IRQn: 优先级 5
- SysTick: 优先级 15

## 注意事项

- 串口使用 USART2，USART2_IRQn 优先级为 5
- APP 必须位于 0x08004000，首字为栈顶地址，第二个字为复位向量
- Flash 写入采用半字对齐方式，奇数长度时最后一个字节留到下次处理
- 跳转前关闭所有中断、SysTick、HAL 外设，然后重置 MSP 并设置 VTOR
- `Core/` 目录由 CubeMX 管理，修改 `.ioc` 文件后会被覆盖
