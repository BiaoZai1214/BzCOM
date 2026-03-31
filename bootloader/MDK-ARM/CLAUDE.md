# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working in the MDK-ARM directory.

## 项目概述

基于 **Keil uVision** 的 STM32F103C8T6 双区备份 OTA Bootloader。

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
├── Interface/               # 核心业务逻辑
│   ├── bootloader.c/h     # Flash 读写、分区切换、跳转
│   ├── iap.c/h             # UART IAP 状态机
│   ├── ota.c/h             # OTA 升级逻辑
│   ├── w25q64.c/h          # W25Q64 Flash 驱动
│   └── at24c64.c/h         # AT24C64 EEPROM 驱动
├── Application/             # 应用层状态机
│   └── App_bootloader.c/h  # 启动流程状态机
└── 1.bootloader/           # 构建输出目录
```

> **注意**: `Core/` 目录由 STM32CubeMX 管理，编辑 `1.bootloader.ioc` 后会重新生成。业务逻辑应写在 `Interface/` 和 `Application/` 中。

## Flash 布局

| 区域 | 地址范围 | 大小 | 说明 |
|------|----------|------|------|
| Bootloader | 0x08000000 - 0x08003FFF | 16KB | 启动引导区 |
| APP_RUN | 0x08004000 - 0x08007FFF | 16KB | 运行区（搬运目标） |
| APP_A | 0x08008000 - 0x0800BFFF | 16KB | A区固件 |
| APP_B | 0x0800C000 - 0x0800FFFF | 16KB | B区固件 |

## 编译构建

使用 Keil uVision 打开 `MDK-ARM/1.bootloader.uvprojx`，按 **F7** 构建。

构建后自动调用 `fromelf.exe` 生成 `.bin` 文件到 `MDK-ARM/1.bootloader/` 目录。

## 启动模式

通过 AT24C64 地址 0x10 处存储的模式值决定（`BootMode_t` 枚举）：

| 模式 | 值 | 说明 |
|------|----|------|
| `MODE_BOOT_UPDATE` | 0x01 | W25Q64中已有固件 |
| `MODE_BOOT_NO_UPDATE` | 0x02 | 不更新，直接启动 |
| `MODE_BOOT_RESET` | 0x03 | 恢复出厂设置 |
| `MODE_BOOT_UART_IAP` | 0x04 | 串口IAP直接写入目标分区 |
| `MODE_BOOT_OTA` | 0x07 | OTA模式：串口→W25Q64→目标分区 |

地址 0x11-0x12 必须为密钥 `0x5A6B` 才生效。

## 双区备份机制

- 激活分区存储在 AT24C64 地址 0x13（0xAA=APP_A，0xBB=APP_B）
- 升级时写入非活跃分区，校验通过后切换激活分区
- 固件校验：栈顶地址必须在 0x20000000-0x20005000

## Bootloader 状态机

1. `App_bootloader_check_update()` - 检查 AT24C64 确定启动模式
2. `App_bootloader_check_default()` - 等待 3 秒按键选择模式（KEY1=OTA，KEY2=UART IAP）
3. `App_bootloader_update()` - 执行升级
4. `App_bootloader_copy_and_jump()` - 复制固件到运行区并跳转

## 关键模块

### bootloader.c (Interface/)

| 函数 | 说明 |
|------|------|
| `Boot_JumpToApp(addr)` | 安全跳转 APP（校验栈顶和入口） |
| `Boot_EraseFlash(addr, pages)` | 擦除 Flash 页 |
| `Boot_CopyFlash(src, dst, size)` | 复制 Flash |
| `Boot_GetActiveBank()` | 获取当前激活分区 |
| `Boot_SetActiveBank(addr)` | 设置激活分区 |
| `Boot_StartUartIap()` | 初始化 UART IAP 接收 |

### iap.c (Interface/)

| 函数 | 说明 |
|------|------|
| `IAP_Start(target_addr)` | 启动 UART IAP |
| `IAP_Process()` | 处理 UART IAP，返回 1=完成 |
| `IAP_IsDone()` | 检查 IAP 是否完成 |
| `IAP_GetReceivedLen()` | 获取已接收字节数 |

### ota.c (Interface/)

| 函数 | 说明 |
|------|------|
| `OTA_Receive()` | OTA 接收处理（串口→W25Q64→Flash 分区） |

### App_bootloader.c (Application/)

| 函数 | 说明 |
|------|------|
| `App_bootloader_check_update()` | 检查 AT24C64 |
| `App_bootloader_check_default()` | 延时等待按键 |
| `App_bootloader_update()` | 执行升级 |
| `App_bootloader_copy_and_jump()` | 复制并跳转 |

## OTA 模式串口协议

1. 发送 `OTA_READY\r\n`
2. 等待收到 `'1'` 启动接收
3. 收到 `'1'` 后回复 `ACK\r\n`
4. 上位机发送 bin 数据，每包回传 `0x00`(ACK) 或 `0x01`(NAK)
5. 空闲超时 2 秒认为传输结束

## 全局变量 (usart.c)

```c
UART_DoubleBuffTypeDef uart_double_buff;  // 256B 双缓冲
volatile uint8_t uart_recv_done;         // 接收完成
volatile uint8_t uart_error_flag;         // 错误标志
volatile uint32_t last_rec_time;          // 上次接收时间
```

## 硬件配置

| 外设 | 引脚 | 配置 |
|------|------|------|
| USART1 | PA9=TX, PA10=RX | 115200bps, 8N1, 空闲中断 |
| SPI1 | PA4=NSS, PA5=SCK, PA6=MISO, PA7=MOSI | W25Q64 |
| I2C1 | PB6=SCL, PB7=SDA | AT24C64 |
| LED_B | PA6 | 低电平点亮 |
| LED_G | PA7 | 低电平点亮 |
| LED_R | PB0 | 低电平点亮 |
| KEY1 | PB13 | 上升沿，进入 OTA 模式 |
| KEY2 | PB12 | 上升沿，进入串口 IAP 模式 |

**系统时钟**: HSE(8MHz) → PLL × 9 = 72MHz

## 中断配置

- NVIC_PRIORITYGROUP_4
- USART1_IRQn: 优先级 0
- SysTick: 优先级 15

## 注意事项

- `Core/` 目录由 CubeMX 管理，修改 `.ioc` 文件后会被覆盖
- 固件必须位于 APP_A 或 APP_B 区，首字为栈顶地址，第二个字为复位向量
- Flash 写入采用半字对齐方式，奇数长度时最后一个字节留到下次处理
- 跳转前关闭所有中断、SysTick，然后重置 MSP 并设置 VTOR
