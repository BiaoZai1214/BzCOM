# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working in the MDK-ARM directory.

## 项目

基于 **Keil uVision** 的 STM32F103C8T6 双区备份 OTA Bootloader。

- **芯片**: STM32F103C8T6 (Cortex-M3, 64KB Flash, 20KB SRAM)
- **HAL库**: STM32Cube HAL F1 V1.8.7
- **输出**: `MDK-ARM/1.bootloader/uart_bootloader.bin`

## 目录结构

```
MDK-ARM/
├── 1.bootloader.uvprojx     # Keil 工程
├── startup_stm32f103xb.s   # 启动文件
├── Interface/               # 业务逻辑
│   ├── bootloader.c/h     # Flash读写、分区切换、跳转
│   ├── ota.c/h             # OTA 升级
│   ├── w25q64.c/h          # W25Q64 Flash
│   └── at24c64.c/h         # AT24C64 EEPROM
├── Application/
│   └── App_bootloader.c/h  # 启动状态机
└── 1.bootloader/           # 构建输出
```

## Flash 布局

| 区域 | 地址范围 | 大小 |
|------|----------|------|
| Bootloader | 0x08000000 - 0x08003FFF | 16KB |
| APP_RUN | 0x08004000 - 0x08007FFF | 16KB |
| APP_A | 0x08008000 - 0x0800BFFF | 16KB |
| APP_B | 0x0800C000 - 0x0800FFFF | 16KB |

## 编译

Keil uVision 打开 `.uvprojx`，按 **F7** 构建，`.bin` 自动生成到 `1.bootloader/`。

## 启动模式（AT24C64 0x10）

| 模式 | 值 | 说明 |
|------|----|------|
| `MODE_BOOT_UPDATE` | 0x01 | W25Q64 中已有固件 |
| `MODE_BOOT_NO_UPDATE` | 0x02 | 直接启动 |
| `MODE_BOOT_RESET` | 0x03 | 恢复出厂 |
| `MODE_BOOT_UART_IAP` | 0x04 | 串口 IAP 直接写入目标分区 |
| `MODE_BOOT_OTA` | 0x07 | 串口→W25Q64→目标分区 |

地址 0x11-0x12 必须为密钥 `0x5A6B` 才生效。

## 双区备份机制

- 激活分区：AT24C64 地址 0x13（0xAA=APP_A，0xBB=APP_B）
- 升级写入非活跃分区，校验通过后切换
- 固件校验：栈顶地址必须在 0x20000000-0x20005000

## 状态机流程

1. `App_bootloader_check_update()` - 检查 AT24C64 确定启动模式
2. `App_bootloader_check_default()` - 等待 3 秒按键（KEY1=OTA，KEY2=UART IAP）
3. `App_bootloader_update()` - 执行升级
4. `App_bootloader_copy_and_jump()` - 复制固件到运行区并跳转

## 关键接口

**bootloader.c**
- `Boot_JumpToApp(addr)` - 安全跳转（校验栈顶）
- `Boot_EraseFlash/CopyFlash` - Flash 操作
- `Boot_GetActiveBank/SetActiveBank` - 分区切换

**ota.c**: `OTA_Receive()` - 串口→W25Q64→Flash 分区

**App_bootloader.c**: `check_update()` / `check_default()` / `update()` / `copy_and_jump()`

## OTA 串口协议

1. 发送 `OTA_READY\r\n`
2. 等待收到 `'1'` 后回复 `ACK\r\n`
3. 上位机发送 bin 数据，每包回传 `0x00`(ACK) 或 `0x01`(NAK)
4. 空闲超时 2 秒认为传输结束

## 硬件配置

| 外设 | 引脚 | 说明 |
|------|------|------|
| USART1 | PA9=TX, PA10=RX | 115200bps, 8N1 |
| SPI1 | PA4=NSS, PA5=SCK, PA6=MISO, PA7=MOSI | W25Q64 |
| I2C1 | PB6=SCL, PB7=SDA | AT24C64 |
| LED_B/G/R | PA6/PA7/PB0 | 低电平点亮 |
| KEY1/KEY2 | PB13/PB12 | 上升沿触发 |

系统时钟：HSE(8MHz) → PLL × 9 = 72MHz

中断：USART1_IRQn 优先级 0，SysTick 优先级 15
