# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

基于 **STM32F103C8T6** 的双区备份 OTA Bootloader，使用 Keil uVision + STM32CubeMX 管理。

- **芯片**: STM32F103C8T6 (Cortex-M3, 64KB Flash, 20KB SRAM)
- **HAL库**: STM32Cube HAL F1 V1.8.7
- **输出**: `MDK-ARM/1.bootloader/uart_bootloader.bin`

## 目录结构

```
OTA-bootloader/
├── Core/                    # STM32CubeMX 管理目录（编辑 .ioc 后重新生成）
│   ├── Src/                 # HAL 外设驱动（usart.c, gpio.c, spi.c, i2c.c...）
│   └── Inc/
├── Drivers/                 # CMSIS + STM32F1xx_HAL_Driver
├── MDK-ARM/                 # Keil uVision 工程（业务逻辑所在）
│   ├── Interface/           # 核心业务逻辑（bootloader.c/h, iap.c/h, ota.c/h...）
│   ├── Application/         # 启动流程状态机（App_bootloader.c/h）
│   └── CLAUDE.md           # 详细文档（Flash布局、启动模式、编译方法等）
└── bootloader.ioc          # STM32CubeMX 配置文件
```

## 重要原则

**业务逻辑写在 `MDK-ARM/Interface/` 和 `MDK-ARM/Application/` 中，不要改 `Core/` 目录。**

`Core/` 目录由 STM32CubeMX 管理，编辑 `bootloader.ioc` 后会被完全覆盖。

## Flash 布局（64KB 总空间）

| 区域 | 地址范围 | 大小 |
|------|----------|------|
| Bootloader | 0x08000000 - 0x08003FFF | 16KB |
| APP_RUN | 0x08004000 - 0x08007FFF | 16KB |
| APP_A | 0x08008000 - 0x0800BFFF | 16KB |
| APP_B | 0x0800C000 - 0x0800FFFF | 16KB |

## 核心模块

- **bootloader.c** - Flash 读写、分区切换、安全跳转
- **ota.c** - OTA 升级流程（串口→W25Q64→Flash 分区）
- **iap.c** - UART IAP 状态机（直接写入目标分区）
- **App_bootloader.c** - 启动流程状态机（模式检测、3秒按键等待、复制跳转）

## 详细文档

详见 `MDK-ARM/CLAUDE.md`，包含：
- 完整编译构建方法（F7 或命令行）
- 启动模式枚举与 AT24C64 存储布局
- 双区备份机制与固件校验规则
- OTA 串口协议细节
- 硬件引脚配置与中断优先级
- 历史调试教训（debug_notes.md）
