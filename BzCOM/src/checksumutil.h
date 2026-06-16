#ifndef CHECKSUMUTIL_H
#define CHECKSUMUTIL_H

#include <QByteArray>
#include <QString>

class CheckSumUtil
{
public:
    enum Type { None = 0, Sum8 = 1, XOR8 = 2, CRC16 = 3 };

    // HEX字符串转字节数组
    static QByteArray parseHex(const QString &input);

    // 计算校验值
    static QByteArray calculate(const QByteArray &data, int type);
};

#endif
