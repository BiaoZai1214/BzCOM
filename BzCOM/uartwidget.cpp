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
    ui->pushButton->setText("选择串口");
    ui->pushButton->setProperty("connected", false);
    ui->bandRateBox->setCurrentText("115200");
    ui->dataBitBox->setCurrentText("8");
    ui->checkBitBox->setCurrentText("None");
    ui->stopBitBox->setCurrentText("1");

    // ========== 修复：添加信号-槽连接 ==========
    connect(ui->pushButton, &QPushButton::clicked, this, &uartWidget::on_pushButton_clicked);
}

uartWidget::~uartWidget()
{
    if (serialPort) { serialPort->close(); delete serialPort; }
    delete ui;
}

bool uartWidget::isSerialConnected() const
{
    return serialPort && serialPort->isOpen()
    && serialPort->isReadable() && serialPort->isWritable();
}

void uartWidget::sendData(const QByteArray &data)
{
    if (!isSerialConnected()) return;
    qint64 written = serialPort->write(data);
    serialPort->flush();
    serialPort->waitForBytesWritten(100);  // 等待数据真正发送出去
    emit dataSent(toHexString(data));
}

QByteArray uartWidget::readAll()
{
    return serialPort ? serialPort->readAll() : QByteArray();
}

// 内部方法
void uartWidget::onDataReady()
{
    if (!isSerialConnected()) return;
    QByteArray data = serialPort->readAll();
    if (!data.isEmpty()) emit dataReceived(data);
}

QString uartWidget::toHexString(const QByteArray &data)
{
    QString s;
    for (int i = 0; i < data.size(); ++i)
        s += QString("%1 ").arg(static_cast<quint8>(data[i]), 2, 16, QChar('0')).toUpper();
    return s.trimmed();
}

QSerialPort::Parity uartWidget::parityFromText(const QString &text)
{
    if (text == "Even") return QSerialPort::EvenParity;
    if (text == "Odd")  return QSerialPort::OddParity;
    return QSerialPort::NoParity;
}

// 按钮：连接/断开
void uartWidget::on_pushButton_clicked(bool)
{
    // 当前已连接 → 断开
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

    // 未连接 → 弹出串口选择对话框
    QDialog dlg(this);
    dlg.setWindowTitle("选择串口");
    dlg.setFixedSize(270, 200);

    auto *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(12, 12, 12, 12);

    auto *list = new QListWidget(&dlg);

    for (const QSerialPortInfo &p : QSerialPortInfo::availablePorts())
        new QListWidgetItem(QString("%1 (%2)").arg(p.description(), p.portName()), list);

    if (list->count() == 0)
        list->addItem("未检测到可用串口！");
    layout->addWidget(list);

    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    auto *btnOk     = new QPushButton("连接", &dlg);
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
        serialPort->setParity(parityFromText(ui->checkBitBox->currentText()));
        serialPort->setStopBits(ui->stopBitBox->currentText() == "2" ? QSerialPort::TwoStop : QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (serialPort->open(QIODevice::ReadWrite)) {
            ui->pushButton->setText("断开");
            ui->pushButton->setProperty("connected", true);
            ui->pushButton->style()->unpolish(ui->pushButton);
            ui->pushButton->style()->polish(ui->pushButton);

            connect(serialPort, &QSerialPort::readyRead, this, &uartWidget::onDataReady);

            // 等待下位机稳定（单片机复位后需要时间初始化）
            serialPort->waitForReadyRead(300);
            // 多次清空串口接收缓冲区（可能包含复位过程中的垃圾数据）
            int cleared = 0;
            QByteArray buf;
            while (serialPort->bytesAvailable() > 0) {
                buf = serialPort->readAll();
                cleared += buf.size();
            }
            // 额外多清一次确保干净
            serialPort->readAll();

            emit systemMessage(QString("串口 %1 已打开 (%2, %3, %4, %5)")
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
