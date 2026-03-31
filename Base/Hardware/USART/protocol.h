/**
 * @file protocol.h
 * @brief 串口自定义协议实现
 *
 * 协议帧格式:
 * +--------+--------+--------+----------+---------+----------+---------+
 * |  帧头   |  命令  |  长度   |  数据区   |  保留    |  CRC16  |
 * | (2B)   | (2B)  |  (2B)  |  (N)    |  (4B)   |  (2B)  |
 * +--------+--------+--------+----------+---------+----------+---------+
 *
 * 长度 = 命令(2) + 长度字段本身(2) + 数据区(N) + 保留(4) = N + 8
 */

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f10x.h"

/*==============================================================================
 * 协议配置
 *============================================================================*/

/* 帧头标识 */
#define PROTOCOL_HEADER_1    0x55
#define PROTOCOL_HEADER_2    0xAA

/* 协议固定字段长度 */
#define PROTOCOL_HEADER_SIZE     2   /* 帧头 */
#define PROTOCOL_CMD_SIZE        2   /* 命令 */
#define PROTOCOL_LEN_SIZE        2   /* 长度 */
#define PROTOCOL_RESERVED_SIZE   4   /* 保留 */
#define PROTOCOL_CRC_SIZE        2   /* CRC */
#define PROTOCOL_FIXED_SIZE      (PROTOCOL_HEADER_SIZE + PROTOCOL_CMD_SIZE + \
                                  PROTOCOL_LEN_SIZE + PROTOCOL_RESERVED_SIZE + PROTOCOL_CRC_SIZE)

/* 最大数据长度 */
#define PROTOCOL_MAX_DATA_SIZE   256

/* 协议缓冲区大小 */
#define PROTOCOL_BUFFER_SIZE     (PROTOCOL_FIXED_SIZE + PROTOCOL_MAX_DATA_SIZE)

/*==============================================================================
 * 协议命令定义
 *============================================================================*/

/* 命令码 (可自定义扩展) */
typedef enum {
    CMD_HEARTBEAT      = 0x0001,  /* 心跳 */
    CMD_QUERY_STATUS   = 0x0002,  /* 查询状态 */
    CMD_UPDATE_START   = 0x0003,  /* 开始升级 */
    CMD_UPDATE_DATA    = 0x0004,  /* 升级数据 */
    CMD_UPDATE_END     = 0x0005,  /* 升级结束 */
    CMD_RESET          = 0x0006,  /* 系统复位 */
    CMD_BOOT_JUMP      = 0x0007,  /* 跳转APP */
    CMD_ACK            = 0x0080,  /* 响应标志(与命令或运算) */
    CMD_NAK            = 0x00FF   /* 错误响应 */
} Protocol_CmdTypeDef;

/*==============================================================================
 * 协议状态
 *============================================================================*/
typedef enum {
    PROTOCOL_STATE_IDLE = 0,      /* 空闲 */
    PROTOCOL_STATE_HEADER,         /* 等待帧头 */
    PROTOCOL_STATE_CMD,            /* 等待命令 */
    PROTOCOL_STATE_LEN,           /* 等待长度 */
    PROTOCOL_STATE_DATA,          /* 等待数据 */
    PROTOCOL_STATE_RESERVED,       /* 等待保留 */
    PROTOCOL_STATE_CRC,           /* 等待CRC */
    PROTOCOL_STATE_COMPLETE       /* 接收完成 */
} Protocol_StateTypeDef;

/*==============================================================================
 * 协议帧结构
 *============================================================================*/
typedef struct {
    uint16_t cmd;                 /* 命令 */
    uint16_t len;                  /* 数据长度 */
    uint8_t  data[PROTOCOL_MAX_DATA_SIZE];  /* 数据区 */
    uint8_t  reserved[PROTOCOL_RESERVED_SIZE]; /* 保留 */
    uint16_t crc;                 /* CRC校验值 */
} Protocol_FrameTypeDef;

/*==============================================================================
 * 协议回调函数类型
 *============================================================================*/
typedef void (*Protocol_CallbackTypeDef)(Protocol_FrameTypeDef *frame);

/*==============================================================================
 * 协议句柄结构
 *============================================================================*/
typedef struct {
    /* 状态机 */
    Protocol_StateTypeDef state;
    uint16_t rx_count;            /* 已接收字节计数 */

    /* 帧缓冲 */
    uint8_t  buffer[PROTOCOL_BUFFER_SIZE];
    uint16_t buffer_len;          /* 帧总长度 */

    /* 解析出的帧 */
    Protocol_FrameTypeDef frame;

    /* 回调函数 */
    Protocol_CallbackTypeDef callback;

    /* 统计 */
    uint32_t rx_total;            /* 总接收帧数 */
    uint32_t rx_error;             /* 错误帧数 */
} Protocol_TypeDef;

/*==============================================================================
 * 外部函数声明
 *============================================================================*/

/**
 * @brief 初始化协议
 * @param p 协议句柄
 * @param callback 接收到完整帧时的回调
 */
void Protocol_Init(Protocol_TypeDef *p, Protocol_CallbackTypeDef callback);

/**
 * @brief 处理接收到的字节
 * @param p 协议句柄
 * @param byte 接收的字节
 * @return 1:接收到完整帧 0:继续接收
 */
uint8_t Protocol_ProcessByte(Protocol_TypeDef *p, uint8_t byte);

/**
 * @brief 发送协议帧
 * @param p 协议句柄
 * @param cmd 命令
 * @param data 数据
 * @param len 数据长度
 */
void Protocol_SendFrame(Protocol_TypeDef *p, uint16_t cmd, uint8_t *data, uint16_t len);

/**
 * @brief 发送响应帧
 * @param p 协议句柄
 * @param cmd 原命令
 * @param ack 1:ACK 0:NAK
 * @param data 数据
 * @param len 数据长度
 */
void Protocol_SendResponse(Protocol_TypeDef *p, uint16_t cmd, uint8_t ack, uint8_t *data, uint16_t len);

/**
 * @brief CRC16 计算 (Modbus RTU)
 * @param data 数据
 * @param len 数据长度
 * @return CRC16值
 */
uint16_t Protocol_CalcCRC16(uint8_t *data, uint16_t len);

#endif
