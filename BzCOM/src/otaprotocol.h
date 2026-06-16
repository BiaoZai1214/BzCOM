#ifndef OTAPROTOCOL_H
#define OTAPROTOCOL_H

#include <QByteArray>

namespace Cmd {
    constexpr quint16 Heartbeat   = 0x0001;
    constexpr quint16 QueryStatus = 0x0002;
    constexpr quint16 UpdateStart = 0x0003;
    constexpr quint16 UpdateData  = 0x0004;
    constexpr quint16 UpdateEnd   = 0x0005;
    constexpr quint16 Reset       = 0x0006;
    constexpr quint16 BootJump    = 0x0007;

    inline quint16 ack(quint16 c) { return c | 0x0080; }
    inline quint16 nak(quint16 c) { return c | 0x00FF; }
}

namespace Proto {
    constexpr quint8  Head1     = 0x55;
    constexpr quint8  Head2     = 0xAA;
    constexpr int     FixedLen  = 10;
    constexpr int     CrcLen    = 2;
}

class OtaProtocol
{
public:
    static OtaProtocol& instance();

    QByteArray buildFrame(quint16 cmd, const QByteArray &data);
    QByteArray parseFrame(QByteArray &buffer);
    bool verifyCRC(const QByteArray &frame);
    quint16 getCmd(const QByteArray &frame);
    QByteArray getData(const QByteArray &frame);
    static QByteArray toBytes32(quint32 value);

private:
    OtaProtocol() = default;
    quint16 calcCRC16(const quint8 *data, quint16 len);
    static const quint16 crc16_table[256];
};

#endif
