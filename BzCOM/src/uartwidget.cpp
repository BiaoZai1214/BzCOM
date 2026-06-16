#include "uartwidget.h"
#include "ui_uartwidget.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QApplication>
#include <QSerialPortInfo>
#include <QMessageBox>

uartWidget::uartWidget(QWidget *parent)
    : ComWidget(parent)
    , ui(new Ui::uartWidget)
    , serialPort(nullptr)
{
    ui->setupUi(this);
    ui->pushButton->setProperty("connected", false);
    ui->bandRateBox->setCurrentText("115200");

    connect(ui->pushButton, &QPushButton::clicked, this, &uartWidget::onConnectClicked);
}

uartWidget::~uartWidget()
{
    closePort();
    delete ui;
}

void uartWidget::updateButtonState(bool connected)
{
    ui->pushButton->setText(connected ? "断开" : "选择串口");
    ComWidget::updateButtonState(ui->pushButton, connected);
}

void uartWidget::closePort(void)
{
    if (!serialPort) return;
    // 串口对象只在这里集中关闭和销毁，减少多处分支重复释放的风险。
    serialPort->close();
    delete serialPort;
    serialPort = nullptr;
}

bool uartWidget::isConnected(void) const
{
    return serialPort && serialPort->isOpen() && serialPort->isReadable() && serialPort->isWritable();
}

void uartWidget::sendData(const QByteArray &data)
{
    if (!isConnected()) return;
    qint64 written = 0;
    while (written < data.size()) {
        qint64 ret = serialPort->write(data.constData() + written, data.size() - written);
        if (ret < 0) {
            emit systemMessage(QString("串口发送失败：%1").arg(serialPort->errorString()));
            return;
        }
        written += ret;
    }
    serialPort->flush();
}

QByteArray uartWidget::clearRxBuffer(void)
{
    return serialPort ? serialPort->readAll() : QByteArray();
}

void uartWidget::onDataReady(void)
{
    if (!isConnected()) return;
    QByteArray data = serialPort->readAll();
    if (!data.isEmpty()) emit dataReceived(data);
}

void uartWidget::onConnectClicked(void)
{
    if (serialPort && serialPort->isOpen()) {
        QString name = serialPort->portName();
        closePort();
        updateButtonState(false);
        emit systemMessage(QString("串口 %1 已断开").arg(name));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("选择串口");
    dlg.setFixedSize(270, 200);

    auto *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(12, 12, 12, 12);

    auto *list = new QListWidget(&dlg);
    for (const QSerialPortInfo &p : QSerialPortInfo::availablePorts())
        new QListWidgetItem(QString("%1 (%2)").arg(p.description(), p.portName()), list);
    if (list->count() == 0) list->addItem("未检测到可用串口！");
    layout->addWidget(list);

    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    auto *btnOk = new QPushButton("连接", &dlg);
    auto *btnCancel = new QPushButton("取消", &dlg);
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnCancel);
    layout->addLayout(btnRow);

    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnOk, &QPushButton::clicked, [&]() {
        auto *item = list->currentItem();
        if (!item) { QMessageBox::warning(this, "提示", "请先选择一个串口！"); return; }

        QString portName = item->text().section('(', 1).section(')', 0, 0);
        closePort();

        // 每次重建一个新的 QSerialPort，避免旧对象遗留配置或连接状态。
        serialPort = new QSerialPort(portName, this);
        serialPort->setBaudRate(ui->bandRateBox->currentText().toInt());

        const QString dataBit = ui->dataBitBox->currentText();
        const QString checkBit = ui->checkBitBox->currentText();
        const QString stopBit = ui->stopBitBox->currentText();

        serialPort->setDataBits(dataBit == "7" ? QSerialPort::Data7 : QSerialPort::Data8);
        serialPort->setParity(checkBit == "Even" ? QSerialPort::EvenParity
                              : checkBit == "Odd" ? QSerialPort::OddParity
                              : QSerialPort::NoParity);
        serialPort->setStopBits(stopBit == "2" ? QSerialPort::TwoStop : QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (serialPort->open(QIODevice::ReadWrite)) {
            updateButtonState(true);
            connect(serialPort, &QSerialPort::readyRead, this, &uartWidget::onDataReady);

            // 新打开串口时主动清一次残留输入，避免把历史数据当成当前会话内容。
            QApplication::processEvents(QEventLoop::AllEvents, 50);
            int cleared = serialPort->readAll().size();

            emit systemMessage(QString("串口 %1 已打开 (%2,%3,%4,%5) 已清空 %6 字节残留数据")
                                   .arg(portName, ui->bandRateBox->currentText(),
                                        ui->dataBitBox->currentText(),
                                        ui->checkBitBox->currentText(),
                                        ui->stopBitBox->currentText(),
                                        QString::number(cleared)));
        } else {
            QMessageBox::critical(this, "错误", "串口打开失败：" + serialPort->errorString());
            closePort();
        }
        dlg.accept();
    });

    dlg.exec();
}
