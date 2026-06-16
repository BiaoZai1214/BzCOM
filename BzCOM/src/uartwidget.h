#ifndef UARTWIDGET_H
#define UARTWIDGET_H

#include "comwidget.h"
#include <QSerialPort>

namespace Ui { class uartWidget; }

class uartWidget : public ComWidget
{
    Q_OBJECT

public:
    explicit uartWidget(QWidget *parent = nullptr);
    ~uartWidget() override;

    bool isConnected(void) const override;
    void sendData(const QByteArray &data) override;
    QByteArray clearRxBuffer() override;


private:
    void updateButtonState(bool connected);
    void closePort(void);
    void onDataReady(void);
    void onConnectClicked(void);

    Ui::uartWidget *ui;
    QSerialPort *serialPort;
};

#endif
