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

    // ---------- 公有接口 ----------
    bool      isSerialConnected() const;
    void      sendData(const QByteArray &data);
    QByteArray readAll();

signals:
    void dataReceived(const QByteArray &data);
    void dataSent(const QString &hexString);
    void systemMessage(const QString &msg);

private:
    // ---------- 内部方法 ----------
    void    onDataReady();
    QString toHexString(const QByteArray &data);

    static QSerialPort::Parity parityFromText(const QString &text);

    // ---------- 按钮点击 ----------
    void on_pushButton_clicked(bool checked = false);

    // ---------- 成员变量 ----------
    Ui::uartWidget *ui;
    QSerialPort    *serialPort;
};

#endif // UARTWIDGET_H
