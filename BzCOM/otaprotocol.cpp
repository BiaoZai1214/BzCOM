/**
 * @file otaprotocol.cpp
 * @brief OTA 升级协议实现
 * @note 基于 Modbus CRC16-2-2 校验的协议帧
 */
#include "otaprotocol.h"

/*============================================================*/
/* Modbus CRC16 查表法                                       */
/*============================================================*/
const quint16 OtaProtocol::crc16_table[256] = {
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

/*============================================================*/
/* 单例模式                                                   */
/*============================================================*/
OtaProtocol &OtaProtocol::instance()
{
    static OtaProtocol instance;
    return instance;
}

/*============================================================*/
/* 构建协议帧                                                 */
/*============================================================*/
/**
 * 帧格式:
 * +--------+--------+--------+----------+---------+----------+---------+
 * |  帧头   |  命令   |  长度  |  数据区   |  保留    |  CRC16  |
 * | 0x55AA |  CMD   |  LEN   |   DATA   | 0x0000  | CRC16   |
 * +--------+--------+--------+----------+---------+----------+---------+
 *   2B       2B       2B       N          4B        2B
 *
 * CRC16计算范围: CMD(2B) + LEN(2B) + DATA(NB) + RESERVE(4B) = LEN + 6字节
 */
QByteArray OtaProtocol::buildFrame(quint16 cmd, const QByteArray &data)
{
    QByteArray frame;
    quint16 len = data.size();

    // 帧头
    frame.reserve(2 + 2 + 2 + len + 4 + 2);
    frame.append(static_cast<char>(PROTOCOL_HEAD_1));
    frame.append(static_cast<char>(PROTOCOL_HEAD_2));

    // 命令码 (小端序)
    frame.append(static_cast<char>(cmd & 0xFF));
    frame.append(static_cast<char>((cmd >> 8) & 0xFF));

    // 长度 (小端序)
    frame.append(static_cast<char>(len & 0xFF));
    frame.append(static_cast<char>((len >> 8) & 0xFF));

    // 数据区
    frame.append(data);

    // 保留字段 (4字节全0)
    frame.append(static_cast<char>(0x00));
    frame.append(static_cast<char>(0x00));
    frame.append(static_cast<char>(0x00));
    frame.append(static_cast<char>(0x00));

    // CRC16计算: 从LEN到RESERVE结束，共(len + 6)字节
    // 即: CMD(2B) + LEN(2B) + DATA(NB) + RESERVE(4B)
    quint16 crc = calcCRC16(reinterpret_cast<const quint8 *>(frame.constData() + 2),
                            len + 6);

    // CRC16 (小端序)
    frame.append(static_cast<char>(crc & 0xFF));
    frame.append(static_cast<char>((crc >> 8) & 0xFF));

    return frame;
}

/*============================================================*/
/* 解析协议帧                                                 */
/*============================================================*/
/**
 * @brief 从接收缓冲中提取完整帧
 * @param buffer 接收缓冲（会被修改，提取后的残留在buffer中）
 * @return 完整帧数据，空表示不完整
 */
QByteArray OtaProtocol::parseFrame(QByteArray &buffer)
{
    // 需要至少10字节才能解析 (HEAD + CMD + LEN + RESERVE + CRC)
    while (buffer.size() >= PROTOCOL_FIXED_LEN + PROTOCOL_CRC_LEN) {
        // 查找帧头 0x55 0xAA
        int headPos = buffer.indexOf(PROTOCOL_HEAD_1);
        if (headPos < 0 || headPos + 1 >= buffer.size()) {
            // 没有找到帧头，清空缓冲
            buffer.clear();
            break;
        }

        // 检查帧头第二个字节
        if (static_cast<quint8>(buffer[headPos + 1]) != PROTOCOL_HEAD_2) {
            // 帧头不匹配，删除第一个字节继续查找
            buffer.remove(0, headPos + 1);
            continue;
        }

        // 确认有足够的字节读取长度
        if (headPos + 4 >= buffer.size()) {
            // 数据不完整，等待更多数据
            break;
        }

        // 提取长度 (小端序)
        quint16 dataLen = static_cast<quint8>(buffer[headPos + 3]) |
                         (static_cast<quint8>(buffer[headPos + 4]) << 8);

        // 计算完整帧长度: HEAD(2) + CMD(2) + LEN(2) + DATA(N) + RESERVE(4) + CRC(2)
        quint16 frameLen = 2 + 2 + 2 + dataLen + 4 + 2;

        if (buffer.size() < headPos + frameLen) {
            // 数据不完整，等待更多数据
            break;
        }

        // 提取完整帧
        QByteArray frame = buffer.mid(headPos, frameLen);

        // 移除已处理的数据
        buffer.remove(0, headPos + frameLen);

        // 验证CRC
        if (verifyCRC(frame)) {
            return frame;
        }

        // CRC校验失败，删除帧头继续查找
        buffer.remove(0, headPos + 2);
    }

    return QByteArray();
}

/*============================================================*/
/* CRC校验                                                    */
/*============================================================*/
/**
 * @brief 校验CRC
 * @param frame 完整帧
 * @return true=校验通过
 */
bool OtaProtocol::verifyCRC(const QByteArray &frame)
{
    if (frame.size() < PROTOCOL_FIXED_LEN + PROTOCOL_CRC_LEN) {
        return false;
    }

    // 提取接收到的CRC (最后2字节，小端序)
    quint16 rxCrc = static_cast<quint8>(frame[frame.size() - 2]) |
                   (static_cast<quint8>(frame[frame.size() - 1]) << 8);

    // 提取数据长度
    quint16 dataLen = static_cast<quint8>(frame[3]) | (static_cast<quint8>(frame[4]) << 8);

    // 计算CRC: 从CMD到RESERVE结束，共(len + 6)字节
    // 即: CMD(2B) + LEN(2B) + DATA(NB) + RESERVE(4B)
    quint16 calcCrc = calcCRC16(reinterpret_cast<const quint8 *>(frame.constData() + 2),
                                dataLen + 6);

    return rxCrc == calcCrc;
}

/*============================================================*/
/* 提取命令码                                                 */
/*============================================================*/
quint16 OtaProtocol::getCmd(const QByteArray &frame)
{
    if (frame.size() < 4) return 0;
    return static_cast<quint8>(frame[2]) | (static_cast<quint8>(frame[3]) << 8);
}

/*============================================================*/
/* 提取数据区                                                 */
/*============================================================*/
QByteArray OtaProtocol::getData(const QByteArray &frame)
{
    if (frame.size() < PROTOCOL_FIXED_LEN) return QByteArray();

    // 提取数据长度
    quint16 dataLen = static_cast<quint8>(frame[3]) | (static_cast<quint8>(frame[4]) << 8);

    // 数据区从第6字节开始
    if (frame.size() < 6 + dataLen) return QByteArray();

    return frame.mid(6, dataLen);
}

/*============================================================*/
/* CRC16计算 (Modbus RTU查表法)                               */
/*============================================================*/
quint16 OtaProtocol::calcCRC16(const quint8 *data, quint16 len)
{
    quint16 crc = 0xFFFF;
    while (len--) {
        crc = (crc >> 8) ^ crc16_table[(crc ^ *data++) & 0xFF];
    }
    return crc;
}

/*============================================================*/
/* 辅助函数                                                   */
/*============================================================*/
QByteArray OtaProtocol::toBytes32(quint32 value)
{
    QByteArray bytes(4, 0);
    bytes[0] = static_cast<char>((value >> 0) & 0xFF);
    bytes[1] = static_cast<char>((value >> 8) & 0xFF);
    bytes[2] = static_cast<char>((value >> 16) & 0xFF);
    bytes[3] = static_cast<char>((value >> 24) & 0xFF);
    return bytes;
}
