# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

STM32F103C8 OTA bootloader 项目，使用 Keil MDK (uVision5) 开发。

## 芯片与工具链

- **芯片**: STM32F103C8 (Cortex-M3, 72MHz)
- **工具链**: Keil MDK 5 (arm5_compiler)
- **编译输出**: `Objects/project.bin` (通过 fromelf 生成)

## 编译命令

```
# Keil uVision 中 Build (F7)
# 或通过命令行:
E:\Software\Keil MDK\ARM\ARMCC\Bin\armcc.exe ...
E:\Software\Keil MDK\ARM\ARMCC\Bin\armlink.exe ...
E:\Software\Keil MDK\ARM\ARM\bin\fromelf.exe --bin -o "Objects/project.bin" "Objects/project.axf"
```

## 目录结构

```
Start/          - 启动文件、时钟配置、芯片头文件 (core_cm3, system_stm32f10x, stm32f10x.h)
User/           - 用户代码 (main.c, Delay, Timer)
Hardware/       - 外设驱动
  LED/          - LED 控制
  KEY/          - 按键驱动
  OLED/         - OLED 显示屏驱动
  USART/        - 串口驱动 (USART1)
  IIC/          - 硬I2C (HardI2C) 和软I2C (SoftI2C)
  TIM/          - 定时器驱动 (TIM1, TIM3, HCSR04, MHSS)
  DMA/          - DMA 控制器
  ADC/          - ADC 驱动 (普通模式, DMA模式)
  SPI/          - 硬SPI (hardSPI) 和软SPI (softSPI), W25Q64 Flash
  LCD/          - LCD 驱动
DebugConfig/    - 调试配置
RTE/            - ARM RTE 组件
```

## 架构特点

### OTA bootloader
`main.c` 中实现中断向量表重映射:
```c
#define APP_START_ADDR 0x08004000
SCB->VTOR = APP_START_ADDR;
```
 bootloader 跳转到 APP 区域执行用户程序。

### 内存布局
- **IROM**: 0x08000000, 64KB (Flash)
- **IRAM**: 0x20000000, 20KB (SRAM)
- **APP区域**: 0x08004000 开始 (bootloader 占用前 16KB)

### 外设说明
- USART1 用于串口通信/调试
- TIM3 用于基础定时/Delay
- SPI1 连接 W25Q64 Flash
- 支持 DMA 传输 (ADC DMA, SPI DMA)

## Keil 项目文件
- `project.uvprojx` - Keil 项目文件
- `project.uvoptx` - Keil 选项配置
- `.vscode/uv4.log.lock` - Keil 进程锁文件

## 注意事项

1. VSCode c_cpp_properties.json 中的 includePath 指向 `e:\Code\Register\Base\` 而非当前 `e:\Code\OTA\Base\`，可能需要修正
2. 所有外设驱动遵循 "xxx.c + xxx.h" 配对命名
3. `stm32f10x.h` 是 CMSIS 设备头文件，定义所有外设寄存器
