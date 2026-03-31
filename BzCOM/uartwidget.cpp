#include "uartwidget.h"
#include "ui_uartwidget.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QMessageBox>

uartWidget::uartWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::uartWidget)
    , serialPort(nullptr)
{
    ui->setupUi(this);
    ui->pushButton->setProperty("connected", false);

    connect(ui->pushButton, &QPushButton::clicked, this, &uartWidget::onPushButtonClicked);
}

uartWidget::~uartWidget()
{
    if (serialPort) { serialPort->close(); delete serialPort; }
    delete ui;
}

bool uartWidget::isSerialConnected() const
{
    return serialPort && serialPort->isOpen() && serialPort->isReadable() && serialPort->isWritable();
}

void uartWidget::sendData(const QByteArray &data)
{
    if (!isSerialConnected()) return;
    serialPort->write(data);
    serialPort->flush();
    serialPort->waitForBytesWritten(100);
}

QByteArray uartWidget::readAll()
{
    return serialPort ? serialPort->readAll() : QByteArray();
}

void uartWidget::onDataReady()
{
    if (!isSerialConnected()) return;
    QByteArray data = serialPort->readAll();
    if (!data.isEmpty()) emit dataReceived(data);
}

void uartWidget::onPushButtonClicked()
{
    if (ui->pushButton->text() == "断开") {
        QString name = serialPort->portName();
        serialPort->close();
        delete serialPort;
        serialPort = nullptr;

        ui->pushButton->setText("选择串口");
        ui->pushButton->setProperty("connected", false);
        ui->pushButton->style()->unpolish(ui->pushButton);
        ui->pushButton->style()->polish(ui->pushButton);
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
        if (serialPort) { serialPort->close(); delete serialPort; }

        serialPort = new QSerialPort(portName, this);
        serialPort->setBaudRate(ui->bandRateBox->currentText().toInt());
        serialPort->setDataBits(ui->dataBitBox->currentText() == "7" ? QSerialPort::Data7 : QSerialPort::Data8);
        serialPort->setParity(ui->checkBitBox->currentText() == "Even" ? QSerialPort::EvenParity
                                      : ui->checkBitBox->currentText() == "Odd" ? QSerialPort::OddParity
                                      : QSerialPort::NoParity);
        serialPort->setStopBits(ui->stopBitBox->currentText() == "2" ? QSerialPort::TwoStop : QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (serialPort->open(QIODevice::ReadWrite)) {
            ui->pushButton->setText("断开");
            ui->pushButton->setProperty("connected", true);
            ui->pushButton->style()->unpolish(ui->pushButton);
            ui->pushButton->style()->polish(ui->pushButton);

            connect(serialPort, &QSerialPort::readyRead, this, &uartWidget::onDataReady);

            serialPort->waitForReadyRead(300);
            int cleared = 0;
            while (serialPort->bytesAvailable() > 0) { cleared += serialPort->readAll().size(); }
            serialPort->readAll();

            emit systemMessage(QString("串口 %1 已打开 (%2,%3,%4,%5) 清除%6字节")
                                   .arg(portName, ui->bandRateBox->currentText(),
                                        ui->dataBitBox->currentText(),
                                        ui->checkBitBox->currentText(),
                                        ui->stopBitBox->currentText())
                                   .arg(cleared));
        } else {
            QMessageBox::critical(this, "错误", "串口打开失败：" + serialPort->errorString());
            delete serialPort;
            serialPort = nullptr;
        }
        dlg.accept();
    });

    dlg.exec();
}
