#ifndef COMWIDGET_H
#define COMWIDGET_H

#include <QWidget>
#include <QByteArray>
#include <QTcpSocket>
#include <QPushButton>
#include <QStyle>

class ComWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ComWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual bool isConnected() const = 0;
    virtual void sendData(const QByteArray &data) = 0;
    virtual QByteArray clearRxBuffer() { return {}; }
    virtual void disconnectDevice() {}

signals:
    void dataReceived(const QByteArray &data);
    void systemMessage(const QString &msg);

protected:
    static void zapSocket(QTcpSocket *&sock) {
        if (!sock) return;
        sock->disconnect();
        sock->abort();
        sock->deleteLater();
        sock = nullptr;
    }

    static void updateButtonState(QPushButton *btn, bool connected) {
        btn->setProperty("connected", connected);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
};

#endif
