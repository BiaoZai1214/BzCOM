/**
 * @file protocol.h
 * @brief 协议帧定义与处理
 * @note 基于 Modbus CRC16-2-2 校验的协议帧
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/*============================================================*/
/* 协议常量                                                   */
/*============================================================*/
#define PROTOCOL_HEAD_1     0x55    // 帧头字节1
#define PROTOCOL_HEAD_2     0xAA    // 帧头字节2

/* 命令码 */
#define CMD_HEARTBEAT       0x0001  // 心跳
#define CMD_QUERY_STATUS    0x0002  // 查询状态
#define CMD_UPDATE_START    0x0003  // 升级开始
#define CMD_UPDATE_DATA     0x0004  // 升级数据
#define CMD_UPDATE_END      0x0005  // 升级结束
#define CMD_RESET           0x0006  // 系统复位
#define CMD_BOOT_JUMP       0x0007  // 跳转APP

/* 响应码 */
#define CMD_ACK(cmd)        ((cmd) | 0x0080)
#define CMD_NAK(cmd)        ((cmd) | 0x00FF)

/* 协议状态 */
#define PROTOCOL_STATE_IDLE     0
#define PROTOCOL_STATE_HEADER   1
#define PROTOCOL_STATE_CMD      2
#define PROTOCOL_STATE_LEN      3
#define PROTOCOL_STATE_DATA     4
#define PROTOCOL_STATE_RESERVE  5
#define PROTOCOL_STATE_CRC      6

/* 协议帧最大长度 */
#define PROTOCOL_MAX_SIZE   272     // HEAD(2) + CMD(2) + LEN(2) + DATA(258) + RESERVE(4) + CRC(2)
#define PROTOCOL_MAX_DATA   258     // 最大数据区

/* 协议帧结构 */
typedef struct {
    uint16_t cmd;        // 命令码
    uint16_t len;        // 数据长度
    uint8_t  data[PROTOCOL_MAX_DATA]; // 数据区
} Protocol_Frame_t;

/* 协议处理器 */
typedef struct {
    uint8_t  state;          // 状态机状态
    uint8_t  buffer[PROTOCOL_MAX_SIZE]; // 缓冲
    uint16_t rx_count;       // 已接收字节数
    uint16_t frame_len;      // 完整帧长度
    Protocol_Frame_t frame;  // 解析后的帧
} Protocol_Handler_t;

/*============================================================*/
/* 协议处理函数                                               */
/*============================================================*/

/**
 * @brief 初始化协议处理器
 * @param h 协议处理器指针
 */
void Protocol_Init(Protocol_Handler_t *h);

/**
 * @brief 处理接收到的字节
 * @param h 协议处理器指针
 * @param byte 接收到的字节
 * @return 1=完整帧已接收 0=继续接收
 */
uint8_t Protocol_ProcessByte(Protocol_Handler_t *h, uint8_t byte);

/**
 * @brief 发送协议帧
 * @param cmd 命令码
 * @param data 数据
 * @param len 数据长度
 */
void Protocol_SendFrame(uint16_t cmd, const uint8_t *data, uint16_t len);

/**
 * @brief 发送ACK响应
 * @param cmd 原命令码
 */
void Protocol_SendACK(uint16_t cmd);

/**
 * @brief 发送NAK响应
 * @param cmd 原命令码
 */
void Protocol_SendNAK(uint16_t cmd);

/**
 * @brief 计算CRC16 (Modbus RTU查表法)
 * @param data 数据
 * @param len 长度
 * @return CRC16值
 */
uint16_t Protocol_CalcCRC16(const uint8_t *data, uint16_t len);

#endif // PROTOCOL_H
