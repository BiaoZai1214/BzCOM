/**
 * @file protocol.c
 * @brief 串口自定义协议实现
 */

#include "protocol.h"
#include "usart.h"

/*==============================================================================
 * CRC16 表 (Modbus RTU)
 *============================================================================*/
static const uint16_t s_crc_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/*==============================================================================
 * CRC16 计算
 *============================================================================*/
uint16_t Protocol_CalcCRC16(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    while (len--) {
        crc = (crc >> 8) ^ s_crc_table[(crc ^ *data++) & 0xFF];
    }
    return crc;
}

/*==============================================================================
 * 初始化
 *============================================================================*/
void Protocol_Init(Protocol_TypeDef *p, Protocol_CallbackTypeDef callback)
{
    p->state = PROTOCOL_STATE_IDLE;
    p->rx_count = 0;
    p->buffer_len = 0;
    p->callback = callback;
    p->rx_total = 0;
    p->rx_error = 0;
}

/*==============================================================================
 * 处理接收字节 (状态机)
 *============================================================================*/
uint8_t Protocol_ProcessByte(Protocol_TypeDef *p, uint8_t byte)
{
    uint8_t ret = 0;

    switch (p->state)
    {
        /* 空闲 -> 等待帧头第一个字节 */
        case PROTOCOL_STATE_IDLE:
            p->rx_count = 0;
            p->buffer[p->rx_count++] = byte;
            p->state = PROTOCOL_STATE_HEADER;
            break;

        /* 等待帧头第二个字节 */
        case PROTOCOL_STATE_HEADER:
            p->buffer[p->rx_count++] = byte;
            if (p->buffer[0] == PROTOCOL_HEADER_1 &&
                p->buffer[1] == PROTOCOL_HEADER_2) {
                p->state = PROTOCOL_STATE_CMD;
            } else {
                /* 帧头错误，重新搜索 */
                p->rx_error++;
                p->state = PROTOCOL_STATE_IDLE;
            }
            break;

        /* 接收命令 (2字节) */
        case PROTOCOL_STATE_CMD:
            p->buffer[p->rx_count++] = byte;
            if (p->rx_count >= PROTOCOL_HEADER_SIZE + PROTOCOL_CMD_SIZE) {
                p->state = PROTOCOL_STATE_LEN;
            }
            break;

        /* 接收长度 (2字节) */
        case PROTOCOL_STATE_LEN:
            p->buffer[p->rx_count++] = byte;
            if (p->rx_count >= PROTOCOL_HEADER_SIZE + PROTOCOL_CMD_SIZE + PROTOCOL_LEN_SIZE) {
                /* 提取数据长度 */
                p->frame.len = (uint16_t)p->buffer[4] | ((uint16_t)p->buffer[5] << 8);

                /* 长度校验 */
                if (p->frame.len > PROTOCOL_MAX_DATA_SIZE) {
                    p->rx_error++;
                    p->state = PROTOCOL_STATE_IDLE;
                    break;
                }

                /* 计算帧总长度 */
                p->buffer_len = PROTOCOL_HEADER_SIZE + PROTOCOL_CMD_SIZE +
                               PROTOCOL_LEN_SIZE + p->frame.len +
                               PROTOCOL_RESERVED_SIZE + PROTOCOL_CRC_SIZE;

                if (p->frame.len == 0) {
                    /* 无数据区，直接进入保留字段 */
                    p->state = PROTOCOL_STATE_RESERVED;
                } else {
                    p->state = PROTOCOL_STATE_DATA;
                }
            }
            break;

        /* 接收数据区 */
        case PROTOCOL_STATE_DATA:
            p->buffer[p->rx_count++] = byte;
            if (p->rx_count >= PROTOCOL_HEADER_SIZE + PROTOCOL_CMD_SIZE +
                           PROTOCOL_LEN_SIZE + p->frame.len) {
                p->state = PROTOCOL_STATE_RESERVED;
            }
            break;

        /* 接收保留字段 (4字节) */
        case PROTOCOL_STATE_RESERVED:
            p->buffer[p->rx_count++] = byte;
            if (p->rx_count >= PROTOCOL_HEADER_SIZE + PROTOCOL_CMD_SIZE +
                           PROTOCOL_LEN_SIZE + p->frame.len + PROTOCOL_RESERVED_SIZE) {
                p->state = PROTOCOL_STATE_CRC;
            }
            break;

        /* 接收CRC (2字节) */
        case PROTOCOL_STATE_CRC:
            p->buffer[p->rx_count++] = byte;
            if (p->rx_count >= p->buffer_len) {
                /* 提取CRC */
                uint16_t rx_crc = p->buffer[p->buffer_len - 2] |
                                 ((uint16_t)p->buffer[p->buffer_len - 1] << 8);

                /* 计算CRC */
                uint16_t calc_crc = Protocol_CalcCRC16(
                    &p->buffer[PROTOCOL_HEADER_SIZE],
                    p->buffer_len - PROTOCOL_HEADER_SIZE - PROTOCOL_CRC_SIZE
                );

                /* 校验 */
                if (rx_crc == calc_crc) {
                    /* 解析帧 */
                    p->frame.cmd = p->buffer[2] | ((uint16_t)p->buffer[3] << 8);
                    if (p->frame.len > 0) {
                        for (uint16_t i = 0; i < p->frame.len; i++) {
                            p->frame.data[i] = p->buffer[PROTOCOL_HEADER_SIZE +
                                                          PROTOCOL_CMD_SIZE +
                                                          PROTOCOL_LEN_SIZE + i];
                        }
                    }

                    /* 调用回调 */
                    if (p->callback) {
                        p->callback(&p->frame);
                    }

                    p->rx_total++;
                    ret = 1;
                } else {
                    p->rx_error++;
                }

                p->state = PROTOCOL_STATE_IDLE;
            }
            break;

        default:
            p->state = PROTOCOL_STATE_IDLE;
            break;
    }

    return ret;
}

/*==============================================================================
 * 发送协议帧
 *============================================================================*/
void Protocol_SendFrame(Protocol_TypeDef *p, uint16_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t tx_buf[PROTOCOL_BUFFER_SIZE];
    uint16_t idx = 0;

    /* 帧头 */
    tx_buf[idx++] = PROTOCOL_HEADER_1;
    tx_buf[idx++] = PROTOCOL_HEADER_2;

    /* 命令 */
    tx_buf[idx++] = cmd & 0xFF;
    tx_buf[idx++] = (cmd >> 8) & 0xFF;

    /* 长度 (数据区长度) */
    tx_buf[idx++] = len & 0xFF;
    tx_buf[idx++] = (len >> 8) & 0xFF;

    /* 数据区 */
    if (len > 0 && data != 0) {
        for (uint16_t i = 0; i < len; i++) {
            tx_buf[idx++] = data[i];
        }
    }

    /* 保留 (4字节) */
    for (uint8_t i = 0; i < PROTOCOL_RESERVED_SIZE; i++) {
        tx_buf[idx++] = 0x00;
    }

    /* 计算CRC */
    uint16_t crc = Protocol_CalcCRC16(&tx_buf[PROTOCOL_HEADER_SIZE],
                                      idx - PROTOCOL_HEADER_SIZE);

    /* CRC */
    tx_buf[idx++] = crc & 0xFF;
    tx_buf[idx++] = (crc >> 8) & 0xFF;

    /* 发送 */
    HAL_UART_Transmit(&huart1, tx_buf, idx, 100);
}

/*==============================================================================
 * 发送响应帧
 *============================================================================*/
void Protocol_SendResponse(Protocol_TypeDef *p, uint16_t cmd, uint8_t ack,
                          uint8_t *data, uint16_t len)
{
    uint16_t resp_cmd;

    if (ack) {
        resp_cmd = cmd | CMD_ACK;  /* 响应标志 */
    } else {
        resp_cmd = cmd | CMD_NAK;  /* 错误响应 */
    }

    Protocol_SendFrame(p, resp_cmd, data, len);
}
