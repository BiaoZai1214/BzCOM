#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QAbstractSocket>

namespace Ui { class tcpWidget; }

class tcpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit tcpWidget(QWidget *parent = nullptr);
    ~tcpWidget() override;

    // ---------- 模式枚举 ----------
    enum TcpMode {
        TcpClientMode = 0,
        TcpServerMode = 1,
        UdpMode       = 2
    };

    // ---------- 公有接口 ----------
    bool isConnected() const;
    void sendData(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void systemMessage(const QString &msg);
    void connectionStateChanged(bool connected);

private:
    // ---------- 初始化 ----------
    void initUI();
    void initConnections();

    // ---------- 模式切换 / 按钮分发 ----------
    void onModeChanged(int idx);
    void onBtnClicked();

    // ---------- 客户端 ----------
    void doConnect();
    void doDisconnect();

    // ---------- 服务端 ----------
    void doListen();
    void doStopListen();

    // ---------- UDP ----------
    void doUdpBind();
    void doUdpUnbind();

    // ---------- TCP 回调 ----------
    void onNewClient();
    void onClientLeft();
    void onClientReadyRead();
    void onClientError(QAbstractSocket::SocketError error);
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

    // ---------- UDP 回调 ----------
    void onUdpReadyRead();

    // ---------- 工具 ----------
    void reset();
    void refreshUI();

    // ---------- 成员变量 ----------
    Ui::tcpWidget *ui;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;   // 客户端socket
    QTcpSocket *m_clientSocket; // 服务端已连接客户端socket
    QUdpSocket *m_udpSocket;

    TcpMode m_currentMode;
    bool    m_isListening;
    bool    m_isConnected;
    bool    m_isUdpBound;
};

#endif // TCPWIDGET_H
