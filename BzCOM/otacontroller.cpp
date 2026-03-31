#include "otacontroller.h"
#include <QFileInfo>

OTAController::OTAController(QObject *parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &OTAController::onTimeout);
}

OTAController::~OTAController() { stop(); }

void OTAController::stop()
{
    m_timer.stop();
    if (m_file.isOpen()) m_file.close();
    m_state = STATE_IDLE;
    m_buf.clear();
}

void OTAController::startUpdate(const QString &filePath, CheckMode mode)
{
    stop();
    m_mode = mode;
    m_file.setFileName(filePath);

    if (!m_file.open(QIODevice::ReadOnly)) { emit otaFinished(false, "无法打开文件"); return; }
    m_totalSize = m_file.size();
    if (m_totalSize == 0) { m_file.close(); emit otaFinished(false, "文件为空"); return; }

    QFileInfo fi(filePath);
    emit progress(QString("--- 开始发送 (%1) ---").arg(mode == CHECK_NONE ? "无校验" : "CRC16协议"), 0);
    emit progress(QString("文件: %1, 大小: %2 字节").arg(fi.fileName()).arg(m_totalSize), 0);

    if (mode == CHECK_NONE) { sendNoCheck(); return; }

    m_pktNum = 0;
    m_sentSize = 0;
    m_lastBuf.clear();
    sendStartFrame();
}

void OTAController::onDataReceived(const QByteArray &data)
{
    if (m_state == STATE_IDLE || m_mode == CHECK_NONE) return;

    m_buf.append(data);
    m_timer.start(TIMEOUT_MS);

    QByteArray frame = OtaProtocol::instance().parseFrame(m_buf);
    if (frame.isEmpty()) return;
    if (!OtaProtocol::instance().verifyCRC(frame)) { m_buf.clear(); return; }

    quint16 cmd = OtaProtocol::instance().getCmd(frame);
    m_buf.clear();

    switch (m_state) {
        case STATE_START_WAIT:
            if (cmd == (CMD_UPDATE_START | 0x0080)) { emit progress("✓ START ACK", 0); sendNextPacket(); }
            break;
        case STATE_DATA_WAIT:
            if (cmd == (CMD_UPDATE_DATA | 0x0080)) sendNextPacket();
            else if (cmd == (CMD_UPDATE_DATA | 0x00FF)) { emit progress(QString("⚠ NAK，重发第%1包").arg(m_pktNum), 2); resendLastPacket(); }
            break;
        case STATE_END_WAIT:
            if (cmd == (CMD_UPDATE_END | 0x0080)) { emit otaFinished(true, QString("成功，共%1包").arg(m_pktNum)); stop(); }
            break;
        default: break;
    }
}

void OTAController::onTimeout()
{
    const char *msg;
    switch (m_state) {
        case STATE_START_WAIT: msg = "等待START响应超时"; break;
        case STATE_DATA_WAIT:  msg = "等待DATA响应超时"; break;
        case STATE_END_WAIT:   msg = "等待END响应超时"; break;
        default: return;
    }
    emit otaFinished(false, msg);
    stop();
}

void OTAController::sendStartFrame()
{
    QByteArray frame = OtaProtocol::instance().buildFrame(CMD_UPDATE_START, OtaProtocol::toBytes32(m_totalSize));
    emit sendData(frame);
    emit progress(QString("→ 发送START (总大小=%1)").arg(m_totalSize), 1);
    m_state = STATE_START_WAIT;
    m_timer.start(TIMEOUT_MS);
}

void OTAController::sendNextPacket()
{
    if (m_file.atEnd()) { sendEndFrame(); return; }

    m_lastBuf = m_file.read(PACKET_SIZE);
    if (m_lastBuf.isEmpty()) return;

    qint64 offset = m_sentSize;
    m_sentSize += m_lastBuf.size();
    ++m_pktNum;

    QByteArray payload = OtaProtocol::toBytes32(offset);
    payload.append(m_lastBuf);
    QByteArray frame = OtaProtocol::instance().buildFrame(CMD_UPDATE_DATA, payload);
    emit sendData(frame);
    emit progress(QString("→ 第%1包 (偏移=%2, 大小=%3)").arg(m_pktNum).arg(offset).arg(m_lastBuf.size()), 1);

    m_state = STATE_DATA_WAIT;
    m_timer.start(TIMEOUT_MS);
}

void OTAController::resendLastPacket()
{
    qint64 offset = m_sentSize - m_lastBuf.size();
    QByteArray payload = OtaProtocol::toBytes32(offset);
    payload.append(m_lastBuf);
    QByteArray frame = OtaProtocol::instance().buildFrame(CMD_UPDATE_DATA, payload);
    emit sendData(frame);
    m_state = STATE_DATA_WAIT;
    m_timer.start(TIMEOUT_MS);
}

void OTAController::sendEndFrame()
{
    QByteArray frame = OtaProtocol::instance().buildFrame(CMD_UPDATE_END, OtaProtocol::toBytes32(m_totalSize));
    emit sendData(frame);
    emit progress("→ 发送END", 1);
    m_state = STATE_END_WAIT;
    m_timer.start(TIMEOUT_END);
}

void OTAController::sendNoCheck()
{
    int pktNum = 0;
    while (!m_file.atEnd()) {
        QByteArray buf = m_file.read(PACKET_SIZE);
        if (buf.isEmpty()) break;
        emit sendData(buf);
        emit progress(QString("→ 第%1包 (%2字节)").arg(++pktNum).arg(buf.size()), 1);
    }
    m_file.close();
    emit otaFinished(true, QString("完成，共%1包").arg(pktNum));
}
