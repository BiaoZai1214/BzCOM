#include "checksumutil.h"

QByteArray CheckSumUtil::parseHex(const QString &input)
{
    QString cleaned = input;
    cleaned.remove(' ').remove(',').remove("0x", Qt::CaseInsensitive).remove("0X");
    return QByteArray::fromHex(cleaned.toUtf8());
}

QByteArray CheckSumUtil::calculate(const QByteArray &data, int type)
{
    if (data.isEmpty()) return QByteArray();

    switch (type) {
    case Sum8: {
        quint16 sum = 0;
        for (char b : data) sum += static_cast<quint8>(b);
        return QByteArray(1, static_cast<char>(sum & 0xFF));
    }
    case XOR8: {
        quint8 x = 0;
        for (char b : data) x ^= static_cast<quint8>(b);
        return QByteArray(1, static_cast<char>(x));
    }
    case CRC16: {
        quint16 crc = 0xFFFF;
        for (char b : data) {
            crc ^= static_cast<quint8>(b);
            for (int j = 0; j < 8; j++) {
                if (crc & 0x01) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }
        QByteArray result;
        result.append(static_cast<char>(crc & 0xFF));
        result.append(static_cast<char>(crc >> 8));
        return result;
    }
    default: return QByteArray();
    }
}
