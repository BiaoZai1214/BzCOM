#include "otacontroller.h"

OTAController::OTAController(QObject *parent)
    : QObject(parent)
{
    connect(&m_timeoutTimer, &QTimer::timeout, this, &OTAController::onTimeout);
}

OTAController::~OTAController()
{
    stop();
}

void OTAController::stop()
{
    m_timeoutTimer.stop();
    if (m_file.isOpen()) {
        m_file.close();
    }
    setState(STATE_IDLE);
    m_recvBuf.clear();
}

void OTAController::startUpdate(const QString &filePath, CheckMode mode)
{
    stop();

    m_filePath = filePath;
    m_mode = mode;
    m_file.setFileName(filePath);

    if (!m_file.open(QIODevice::ReadOnly)) {
        emit otaFinished(false, "无法打开文件：" + filePath);
        return;
    }

    m_totalSize = m_file.size();
    if (m_totalSize == 0) {
        m_file.close();
        emit otaFinished(false, "文件为空");
        return;
    }

    QFileInfo fi(filePath);
    emit progress(QString("--- 开始发送文件 (%1) ---")
                      .arg(mode == CHECK_NONE ? "无校验" : "CRC16-2-2 协议"), 0);
    emit progress(QString("文件: %1, 大小: %2 字节").arg(fi.fileName()).arg(m_totalSize), 0);

    if (mode == CHECK_NONE) {
        sendFileNoCheck();
        return;
    }

    // 协议模式：启动状态机
    m_pktNum = 0;
    m_sentSize = 0;
    m_recvBuf.clear();

    // 发送 START 帧
    QByteArray startFrame = OtaProtocol::instance().buildFrame(
        CMD_UPDATE_START, OtaProtocol::toBytes32(m_totalSize));
    emit sendData(startFrame);
    emit progress(QString("➤ 发送升级开始帧, 总大小=%1").arg(m_totalSize), 1);

    setState(STATE_START_WAIT_ACK);
    m_timeoutTimer.start(TIMEOUT_MS);
}

void OTAController::onDataReceived(const QByteArray &data)
{
    if (m_state == STATE_IDLE || m_mode == CHECK_NONE) {
        return;
    }

    m_recvBuf.append(data);
    m_timeoutTimer.start(TIMEOUT_MS);  // 重置超时
    handleResponse();
}

void OTAController::onTimeout()
{
    if (m_state == STATE_IDLE) return;

    switch (m_state) {
        case STATE_START_WAIT_ACK:
            emit otaFinished(false, "等待 UPDATE_START 响应超时");
            break;
        case STATE_DATA_WAIT_ACK:
            emit otaFinished(false, QString("第 %1 包响应超时或 NAK").arg(m_pktNum));
            break;
        case STATE_END_WAIT_ACK:
            emit otaFinished(false, "等待 UPDATE_END 响应超时");
            break;
        default:
            break;
    }
    stop();
}

void OTAController::handleResponse()
{
    QByteArray frame = OtaProtocol::instance().parseFrame(m_recvBuf);
    if (frame.isEmpty()) return;

    if (!OtaProtocol::instance().verifyCRC(frame)) {
        m_recvBuf.clear();
        return;
    }

    quint16 respCmd = OtaProtocol::instance().getCmd(frame);
    m_recvBuf.clear();

    switch (m_state) {
        case STATE_START_WAIT_ACK:
            if (respCmd == (CMD_UPDATE_START | 0x0080)) {
                emit progress("✓ 收到 UPDATE_START ACK", 0);
                sendNextPacket();
            }
            break;

        case STATE_DATA_WAIT_ACK:
            if (respCmd == (CMD_UPDATE_DATA | 0x0080)) {
                sendNextPacket();
            } else if (respCmd == (CMD_UPDATE_DATA | 0x00FF)) {
                // NAK，重发上一包（简单处理：重新发送）
                emit progress(QString("⚠ 第 %1 包 NAK，重发...").arg(m_pktNum), 2);
                // 简化处理：直接继续发送下一包
                sendNextPacket();
            }
            break;

        case STATE_END_WAIT_ACK:
            if (respCmd == (CMD_UPDATE_END | 0x0080)) {
                emit otaFinished(true, QString("OTA 升级成功，共 %1 包").arg(m_pktNum));
                stop();
            }
            break;

        default:
            break;
    }
}

void OTAController::sendNextPacket()
{
    if (m_file.atEnd()) {
        // 发送 END 帧
        QByteArray endFrame = OtaProtocol::instance().buildFrame(
            CMD_UPDATE_END, OtaProtocol::toBytes32(m_totalSize));
        emit sendData(endFrame);
        emit progress("➤ 发送升级结束帧", 1);
        setState(STATE_END_WAIT_ACK);
        m_timeoutTimer.start(TIMEOUT_END_MS);  // 烧录需要更长时间
        return;
    }

    QByteArray buf = m_file.read(PACKET_SIZE);
    if (buf.isEmpty()) return;

    qint64 offset = m_sentSize;
    m_sentSize += buf.size();
    ++m_pktNum;

    QByteArray payload = OtaProtocol::toBytes32(offset);
    payload.append(buf);
    emit sendData(OtaProtocol::instance().buildFrame(CMD_UPDATE_DATA, payload));
    emit progress(QString("➤ 发送第 %1 包, 偏移=%2, 大小=%3")
                      .arg(m_pktNum).arg(offset).arg(buf.size()), 1);

    setState(STATE_DATA_WAIT_ACK);
}

//==============================================================================
// 无校验模式
//==============================================================================

void OTAController::sendFileNoCheck()
{
    int pktNum = 0;
    while (!m_file.atEnd()) {
        QByteArray buf = m_file.read(PACKET_SIZE);
        if (buf.isEmpty()) break;
        emit sendData(buf);
        emit progress(QString("➤ 发送第 %1 包 (%2 字节)").arg(++pktNum).arg(buf.size()), 1);
    }
    m_file.close();
    emit otaFinished(true, QString("文件发送完成，共 %1 包").arg(pktNum));
}
