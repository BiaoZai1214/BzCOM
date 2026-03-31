/**
 * @file otaprotocol.h
 * @brief OTA 升级协议封装
 * @note 基于 Modbus CRC16-2-2 校验的协议帧
 */
#ifndef OTAPROTOCOL_H
#define OTAPROTOCOL_H

#include <QByteArray>
#include <QObject>

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

/* 响应码：CMD | 0x0080 = ACK, CMD | 0x00FF = NAK */
#define CMD_ACK(cmd)        ((cmd) | 0x0080)
#define CMD_NAK(cmd)        ((cmd) | 0x00FF)

/* 协议帧固定长度 */
#define PROTOCOL_FIXED_LEN  10      // HEAD(2) + CMD(2) + LEN(2) + RESERVE(4)
#define PROTOCOL_CRC_LEN    2       // CRC16长度

class OtaProtocol
{
public:
    static OtaProtocol& instance();

    /**
     * @brief 构建协议帧
     * @param cmd 命令码
     * @param data 数据区
     * @return 完整的协议帧
     */
    QByteArray buildFrame(quint16 cmd, const QByteArray &data);

    /**
     * @brief 解析接收数据，提取完整帧
     * @param buffer 接收缓冲（会被修改）
     * @return 完整帧数据，空表示不完整
     */
    QByteArray parseFrame(QByteArray &buffer);

    /**
     * @brief 校验CRC
     * @param frame 完整帧数据
     * @return true=校验通过
     */
    bool verifyCRC(const QByteArray &frame);

    /**
     * @brief 从帧中提取命令码
     * @param frame 完整帧
     * @return 命令码
     */
    quint16 getCmd(const QByteArray &frame);

    /**
     * @brief 从帧中提取数据区
     * @param frame 完整帧
     * @return 数据区
     */
    QByteArray getData(const QByteArray &frame);

    /**
     * @brief 计算CRC16 (Modbus RTU)
     * @param data 数据
     * @param len 长度
     * @return CRC16值
     */
    quint16 calcCRC16(const quint8 *data, quint16 len);

private:
    OtaProtocol() = default;

    // CRC16查表法
    static const quint16 crc16_table[256];
};

#endif // OTAPROTOCOL_H
