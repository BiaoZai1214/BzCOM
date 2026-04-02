# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目

基于 **STM32F103C8T6** 的双区备份 OTA Bootloader，使用 Keil uVision + STM32CubeMX 管理。

## 重要原则

**业务逻辑写在 `MDK-ARM/Interface/` 和 `MDK-ARM/Application/` 中，禁止修改 `Core/` 目录。**

`Core/` 由 STM32CubeMX 管理，编辑 `.ioc` 后会被完全覆盖。

## 详细文档

`MDK-ARM/CLAUDE.md` 包含：Flash布局、编译构建、启动模式、双区备份机制、OTA协议、硬件配置

## 调试教训

`MDK-ARM/docs/debug_notes.md` 关键经验：
- **双缓冲**：uart 回调始终复制到 buff1，rx_len 为"数据就绪"标志，上一包未处理则丢弃新数据
- **HAL宏冲突**：自定义 Flash 宏勿与 `stm32f103xb.h` / `stm32f1xx_hal_flash_ex.h` 同名
- **按键消抖**：50ms，高电平触发对应检测 `GPIO_PIN_SET`
- **IAP 超时**：空闲超时 > 发送间隔（本次 5 秒）
