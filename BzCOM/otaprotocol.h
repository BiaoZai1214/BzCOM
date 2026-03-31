/**
 * @file otaprotocol.h
 * @brief OTA 升级协议封装，基于 Modbus CRC16-2-2 校验
 */
#ifndef OTAPROTOCOL_H
#define OTAPROTOCOL_H

#include <QByteArray>
#include <QObject>

#define PROTOCOL_HEAD_1     0x55
#define PROTOCOL_HEAD_2     0xAA

#define CMD_HEARTBEAT       0x0001
#define CMD_QUERY_STATUS    0x0002
#define CMD_UPDATE_START    0x0003
#define CMD_UPDATE_DATA     0x0004
#define CMD_UPDATE_END      0x0005
#define CMD_RESET           0x0006
#define CMD_BOOT_JUMP       0x0007

#define CMD_ACK(cmd)        ((cmd) | 0x0080)
#define CMD_NAK(cmd)        ((cmd) | 0x00FF)

#define PROTOCOL_FIXED_LEN  10
#define PROTOCOL_CRC_LEN    2

class OtaProtocol
{
public:
    static OtaProtocol& instance();

    QByteArray buildFrame(quint16 cmd, const QByteArray &data);
    QByteArray parseFrame(QByteArray &buffer);
    bool verifyCRC(const QByteArray &frame);
    quint16 getCmd(const QByteArray &frame);
    QByteArray getData(const QByteArray &frame);
    quint16 calcCRC16(const quint8 *data, quint16 len);
    static QByteArray toBytes32(quint32 value);

private:
    OtaProtocol() = default;
    static const quint16 crc16_table[256];
};

#endif
