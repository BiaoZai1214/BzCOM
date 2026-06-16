#ifndef TCPCLIENTWIDGET_H
#define TCPCLIENTWIDGET_H

#include "comwidget.h"
#include <QTcpSocket>

namespace Ui { class tcpclientWidget; }

class tcpclientWidget : public ComWidget
{
    Q_OBJECT

public:
    explicit tcpclientWidget(QWidget *parent = nullptr);
    ~tcpclientWidget() override;

    bool isConnected(void) const override;
    void sendData(const QByteArray &data) override;
    void disconnectDevice() override;

private slots:
    void onConnected(void);
    void onDisconnected(void);
    void onError(QAbstractSocket::SocketError);

private:
    void setControlsEnabled(bool on);

    Ui::tcpclientWidget *ui;
    QTcpSocket *m_socket = nullptr;
};

#endif
