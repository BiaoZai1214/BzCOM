#ifndef UDPWIDGET_H
#define UDPWIDGET_H

#include "comwidget.h"
#include <QUdpSocket>

namespace Ui { class udpWidget; }

class udpWidget : public ComWidget
{
    Q_OBJECT

public:
    explicit udpWidget(QWidget *parent = nullptr);
    ~udpWidget() override;

    bool isConnected(void) const override;
    void sendData(const QByteArray &data) override;
    void disconnectDevice() override;

private:
    Ui::udpWidget *ui;
    QUdpSocket *m_socket;
    quint16 m_localPort = 8888;
    quint16 m_remotePort = 9999;
    QString m_remoteHost;
};

#endif
