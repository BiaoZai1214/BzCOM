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

    enum TcpMode { TcpClientMode = 0, TcpServerMode = 1, UdpMode = 2 };

    bool isConnected() const;
    void sendData(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void systemMessage(const QString &msg);
    void connectionStateChanged(bool connected);

private:
    void onModeChanged(int idx);
    void onBtnClicked();
    void doConnect();
    void doDisconnect();
    void doListen();
    void doStopListen();
    void doUdpBind();
    void doUdpUnbind();
    void onNewClient();
    void onClientLeft();
    void onClientReadyRead();
    void onClientError(QAbstractSocket::SocketError error);
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onUdpReadyRead();
    void reset();
    void refreshUI();

    Ui::tcpWidget *ui;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;
    QTcpSocket *m_clientSocket;
    QUdpSocket *m_udpSocket;
    TcpMode m_currentMode;
    bool m_isListening;
    bool m_isConnected;
    bool m_isUdpBound;
};

#endif
