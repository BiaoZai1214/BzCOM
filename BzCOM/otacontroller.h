#ifndef OTACONTROLLER_H
#define OTACONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QTimer>
#include "otaprotocol.h"

class OTAController : public QObject
{
    Q_OBJECT

public:
    enum CheckMode { CHECK_NONE = 0, CHECK_MODBUS = 1 };
    static constexpr int PACKET_SIZE = 240;
    static constexpr int TIMEOUT_MS = 3000;
    static constexpr int TIMEOUT_END = 9000;

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
    enum State { STATE_IDLE, STATE_START_WAIT, STATE_DATA_WAIT, STATE_END_WAIT };

    void sendStartFrame();
    void sendNextPacket();
    void resendLastPacket();
    void sendEndFrame();
    void sendNoCheck();
    void onTimeout();

    CheckMode m_mode = CHECK_NONE;
    QFile m_file;
    qint64 m_totalSize = 0;
    State m_state = STATE_IDLE;
    int m_pktNum = 0;
    qint64 m_sentSize = 0;
    QByteArray m_buf;
    QByteArray m_lastBuf;
    QTimer m_timer;
};

#endif
