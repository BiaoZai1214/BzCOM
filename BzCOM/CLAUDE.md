# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
BzCOM is a Qt-based GUI communication utility that supports both serial port (UART) and TCP network communication, with features including:
- Text/Hex mode data sending/receiving
- File transfer with ACK-based retry mechanism
- Checksum calculation
- Communication logging with save/clear functionality
- Configurable serial port parameters (baud rate, parity, stop bits, etc.)
- TCP client/server functionality

## Build & Development Commands
This is a Qt 5/6 qmake project:
1. **Generate build files**: `qmake BzCOM.pro`
2. **Build**:
   - Windows (MinGW): `mingw32-make`
   - Linux/macOS: `make`
   - Or build directly via Qt Creator
3. **Clean**: `make clean` or `mingw32-make clean`
4. **Run**: Execute the compiled `BzCOM.exe` (Windows) or `BzCOM` binary in the build output directory

Required Qt modules: `widgets`, `serialport`, `network` (C++11 standard)

## Code Architecture
High-level component structure:
- **MainWindow**: Core application window, handles all shared logic:
  - UI initialization and shared controls (mode switching, log display, send controls, file operations)
  - Common data processing (hex formatting, checksum calculation, packet transmission with retries)
  - Coordinates communication between active connection widget and UI
- **uartWidget**: Standalone serial port communication widget, handles all low-level UART operations
- **tcpWidget**: Standalone TCP network communication widget, handles both client and server TCP connections

All UI layouts are defined in Qt Designer `.ui` files for each component.

## Key Constants & Patterns
Important shared definitions in `mainwindow.h`:
- Packet size: `PACKET_SIZE = 256` (max bytes per transmission packet)
- ACK timeout: `ACK_TIMEOUT_MS = 1000` (1 second timeout for ACK reception)
- ACK byte: configurable via `ackEdit` UI control (empty = no ACK detection)
- Log types: `Sent`, `Received`, `SystemMsg` (used for color-coded log display)
- All data is passed as `QByteArray` between components for consistent processing
