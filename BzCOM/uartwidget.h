#ifndef UARTWIDGET_H
#define UARTWIDGET_H

#include <QWidget>
#include <QSerialPort>

namespace Ui { class uartWidget; }

class uartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit uartWidget(QWidget *parent = nullptr);
    ~uartWidget() override;

    bool isSerialConnected() const;
    void sendData(const QByteArray &data);
    QByteArray readAll();

signals:
    void dataReceived(const QByteArray &data);
    void dataSent(const QString &hexString);
    void systemMessage(const QString &msg);

private:
    void onDataReady();
    void onPushButtonClicked();

    Ui::uartWidget *ui;
    QSerialPort *serialPort;
};

#endif
