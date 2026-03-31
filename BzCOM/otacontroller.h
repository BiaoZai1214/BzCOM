#ifndef OTACONTROLLER_H
#define OTACONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include "otaprotocol.h"

/**
 * @brief OTA 业务控制器
 * @note 独立于 UI，负责 OTA 状态机和文件发送逻辑
 *
 * 信号:
 *   sendData(data)    → 发送数据（MainWindow 转发到串口/TCP）
 *   progress(msg,type) → 进度消息（type: 0=系统蓝,1=发送中蓝,2=失败红）
 *   otaFinished(success, msg) → OTA 结果
 */
class OTAController : public QObject
{
    Q_OBJECT

public:
    enum CheckMode { CHECK_NONE = 0, CHECK_MODBUS = 1 };

    static constexpr int PACKET_SIZE    = 240;
    static constexpr int ACK_TIMEOUT_MS = 3000;

    explicit OTAController(QObject *parent = nullptr);
    ~OTAController() override;

    void startUpdate(const QString &filePath, CheckMode mode);
    void stop();

signals:
    void sendData(const QByteArray &data);
    void progress(const QString &msg, int type);
    void otaFinished(bool success, const QString &msg);

public slots:
    void onDataReceived(const QByteArray &data);

private:
    // OTA 状态机
    enum State {
        STATE_IDLE,
        STATE_START_WAIT_ACK,
        STATE_DATA_SENDING,
        STATE_DATA_WAIT_ACK,
        STATE_END_WAIT_ACK,
    };

    void setState(State s) { m_state = s; }
    void sendNextPacket();
    void sendFileNoCheck();
    void handleResponse();
    void onTimeout();

    CheckMode m_mode = CHECK_NONE;
    QFile m_file;
    QString m_filePath;
    qint64 m_totalSize = 0;

    State m_state = STATE_IDLE;
    int m_pktNum = 0;
    qint64 m_sentSize = 0;

    QByteArray m_recvBuf;
    QTimer m_timeoutTimer;

    static constexpr int TIMEOUT_MS = 3000;
    static constexpr int TIMEOUT_END_MS = 9000;
};

#endif
