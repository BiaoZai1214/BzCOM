#ifndef TCPSERVERWIDGET_H
#define TCPSERVERWIDGET_H

#include "comwidget.h"
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui { class tcpserverWidget; }

class tcpserverWidget : public ComWidget
{
    Q_OBJECT

public:
    explicit tcpserverWidget(QWidget *parent = nullptr);
    ~tcpserverWidget() override;

    bool isConnected(void) const override;
    void sendData(const QByteArray &data) override;
    void disconnectDevice() override;

private slots:
    void onNewClient(void);
    void onClientLeft(void);
    void onClientError(QAbstractSocket::SocketError);

private:
    Ui::tcpserverWidget *ui;
    QTcpServer *m_server;
    QTcpSocket *m_client = nullptr;
};

#endif
