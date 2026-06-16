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
E:\Code\OTA\1.bootloader\
├── 1.bootloader.ioc              # STM32CubeMX 配置文件
├── .mxproject                    # CubeMX 项目元数据
├── Core/                         # HAL 源码 (由 CubeMX 管理)
│   ├── Inc/                     # HAL 头文件
│   └── Src/                     # HAL 驱动
│       ├── main.c               # 入口
│       ├── usart.c              # USART2 初始化
│       ├── gpio.c               # GPIO 初始化
│       └── stm32f1xx_it.c       # 中断处理
├── Drivers/                      # STM32 HAL 驱动库
└── MDK-ARM/                      # Keil 工程目录
    ├── 1.bootloader.uvprojx     # Keil 工程文件
    ├── 1.bootloader.uvoptx      # Keil 选项文件
    ├── startup_stm32f103xb.s    # 启动文件
    ├── Interface/                # Bootloader 核心逻辑
    │   └── bootloader.c/h       # Flash 读写、跳转、安全校验
    └── Application/             # 应用层状态机
        └── App_bootloader.c/h   # bootloader 工作流程状态机
```

## 编译构建

使用 Keil uVision 打开 `MDK-ARM/1.bootloader.uvprojx`，按 **F7** 或点击 **Build**。

构建后自动调用 `fromelf.exe` 生成 `.bin` 文件到 `MDK-ARM/1.bootloader/` 目录。

> **注意**: `Core/` 目录由 STM32CubeMX 管理，编辑 `1.bootloader.ioc` 后会重新生成。业务逻辑应写在 `MDK-ARM/Interface/` 和 `MDK-ARM/Application/` 中。

## Flash 布局

| 区域 | 地址范围 | 大小 |
|------|----------|------|
| Bootloader | 0x08000000 - 0x08003FFF | 16KB |
| APP | 0x08004000 - 0x0800BFFF | 32KB |

> 注意: `bootloader.h` 中定义 `APP_END_ADDR = 0x0800C000`

## 状态机工作流程

1. `BOOTLOADER_STATUS_INIT` - 等待用户发送 `start` 命令
2. `BOOTLOADER_STATUS_RUN` - 擦除 Flash，准备接收
3. `BOOTLOADER_STATUS_REC_DATA` - 接收数据（超时 2s 或 KEY1 中断触发认为接收完成）
4. `BOOTLOADER_STATUS_JUMP_APP` - 跳转到 APP

> `BOOTLOADER_STATUS_CHECK_DATA` 在 enum 中定义但未实际使用

## 关键模块

### bootloader.c (MDK-ARM/Interface/)

| 函数 | 说明 |
|------|------|
| `Int_bootloader_receive_app()` | 启动串口空闲中断接收 |
| `Int_bootloader_erase_flash(page_addr, pages)` | 擦除 Flash 页 |
| `Int_bootloader_jump_to_app()` | 安全跳转到 APP（校验栈顶和入口地址） |
| `HAL_UARTEx_RxEventCallback()` | 串口空闲中断回调，在中断内完成 Flash 写入（半字对齐） |

### App_bootloader.c (MDK-ARM/Application/)

| 函数 | 说明 |
|------|------|
| `App_bootloader_init()` | 打印欢迎信息，初始化状态为 INIT |
| `App_bootloader_work()` | 状态机主循环，内部调用静态状态处理函数 |

> 状态处理函数 `State_Init()`、`State_Run()`、`State_RecData()`、`State_JumpApp()` 为 static，不对外暴露

### main.c (Core/Src/)

- 调用 `App_bootloader_init()` 初始化
- 主循环调用 `App_bootloader_work()` 运行状态机

## 全局变量 (bootloader.c)

```c
extern uint8_t   uart_rec_buff[BOOTLOADER_UART_REC_BUFF_LEN];  // 512B 接收缓冲
extern uint16_t  uart_rec_len;               // 本次接收长度
extern uint16_t  uart_rec_full_len;          // 累计接收长度
extern uint32_t  last_rec_time;              // 上次接收时间（用于超时检测）
extern uint32_t  flash_write_offset;         // Flash 写入偏移
extern uint8_t   last_byte_flag;             // 奇数长度时遗留的字节标记
extern uint8_t   last_byte;                  // 遗留的单字节
```

## 全局变量 (App_bootloader.c)

```c
extern uint8_t   app_rec_start_buff[64];      // 命令解析缓冲
extern uint16_t  app_rec_start_len;           // 命令解析长度
extern uint8_t   flag;                       // KEY1 中断触发标记
```

## 用户协议

1. 发送 `start` 命令启动 bootloader（只检查关键字，不解析长度）
2. 直接发送 bin 文件数据（串口空闲中断接收，每包 512 字节）
3. 按 **KEY1** 手动触发跳转，或等待 **2秒超时** 自动跳转

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
- KEY1(PB13) 下降沿触发外部中断，可在传输过程中手动触发以结束接收
- APP 必须位于 0x08004000，首字为栈顶地址，第二个字为复位向量
- 跳转前校验：栈顶地址高 16 位必须为 0x2000（SRAM 基地址），复位向量必须在 APP_START_ADDR ~ APP_END_ADDR 范围内
- Flash 写入采用半字对齐方式，奇数长度时最后一个字节留到下次处理
- 跳转前关闭所有中断、SysTick、HAL 外设，然后重置 MSP 并设置 VTOR
- `Core/` 目录由 CubeMX 管理，修改 `.ioc` 文件后会被覆盖
