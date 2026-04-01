# BzCOM

Qt-based GUI 通信工具，支持串口（UART）和 TCP 网络通信。

## 功能特性

- **UART 串口通信**：支持配置波特率、校验位、停止位等参数
- **TCP 客户端/服务器**：支持 TCP 连接模式
- **数据收发**：文本/Hex 模式切换
- **文件传输**：基于 ACK 的重传机制
- **校验和计算**：支持多种校验算法
- **通信日志**：支持保存和清除
- **OTA 控制器**：支持 OTA 升级功能

## 构建

### 环境要求

- Qt 5.15+ (MinGW 64-bit)
- C++11

### 编译步骤

```bash
# 生成构建文件
qmake BzCOM.pro

# 编译 (Windows)
mingw32-make

# 清理
mingw32-make clean
```

或使用 Qt Creator 直接打开 `BzCOM.pro` 构建。

## 项目结构

```
BzCOM/
├── main.cpp              # 应用入口
├── mainwindow.cpp/h      # 主窗口
├── uartwidget.cpp/h/ui   # 串口通信组件
├── tcpwidget.cpp/h/ui    # TCP 通信组件
├── otacontroller.cpp/h    # OTA 控制器
├── otaprotocol.cpp/h      # OTA 协议
└── BzCOM.pro              # qmake 项目文件
```

## 使用说明

1. 选择通信模式（UART/TCP）
2. 配置连接参数
3. 打开连接
4. 发送/接收数据

## License

MIT
