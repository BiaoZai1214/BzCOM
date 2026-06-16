#include "tcpclientwidget.h"
#include "ui_tcpclientwidget.h"
#include <QMessageBox>

tcpclientWidget::tcpclientWidget(QWidget *parent)
    : ComWidget(parent)
    , ui(new Ui::tcpclientWidget)
{
    ui->setupUi(this);
    ui->connectBtn->setProperty("connected", false);

    connect(ui->connectBtn, &QPushButton::clicked, this, [this]() {
        if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
            m_socket->disconnectFromHost();
            return;
        }
        QString host = ui->ipEdit->text().trimmed();
        quint16 port = ui->portEdit->text().toUShort();
        if (host.isEmpty() || port == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请输入IP和端口"));
            return;
        }
        zapSocket(m_socket);
        m_socket = new QTcpSocket(this);
        connect(m_socket, &QTcpSocket::connected, this, &tcpclientWidget::onConnected);
        connect(m_socket, &QTcpSocket::disconnected, this, &tcpclientWidget::onDisconnected);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                this, &tcpclientWidget::onError);
        connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
            QByteArray d = m_socket->readAll();
            if (!d.isEmpty()) emit dataReceived(d);
        });
        m_socket->connectToHost(host, port);
        ui->connectBtn->setText(tr("连接中..."));
        ui->connectBtn->setEnabled(false);
    });
}

tcpclientWidget::~tcpclientWidget() {
    zapSocket(m_socket);
    delete ui;
}

bool tcpclientWidget::isConnected(void) const {
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void tcpclientWidget::sendData(const QByteArray &data) {
    if (!isConnected()) { QMessageBox::warning(this, tr("提示"), tr("连接未就绪")); return; }
    qint64 written = m_socket->write(data);
    if (written < 0) emit systemMessage(tr("[错误] 发送失败：%1").arg(m_socket->errorString()));
}

void tcpclientWidget::disconnectDevice() {
    zapSocket(m_socket);
    ui->connectBtn->setText(tr("连接"));
    ComWidget::updateButtonState(ui->connectBtn, false);
    setControlsEnabled(true);
}

void tcpclientWidget::onConnected(void) {
    ui->connectBtn->setText(tr("断开"));
    ComWidget::updateButtonState(ui->connectBtn, true);
    setControlsEnabled(false);
    emit systemMessage(tr("已连接：%1:%2").arg(ui->ipEdit->text()).arg(ui->portEdit->text()));
}

void tcpclientWidget::onDisconnected(void) {
    ui->connectBtn->setText(tr("连接"));
    ComWidget::updateButtonState(ui->connectBtn, false);
    setControlsEnabled(true);
    emit systemMessage(tr("已断开连接"));
}

void tcpclientWidget::onError(QAbstractSocket::SocketError) {
    QString err = m_socket ? m_socket->errorString() : tr("连接失败");
    emit systemMessage(tr("[错误] %1").arg(err));
    zapSocket(m_socket);
    ui->connectBtn->setText(tr("连接"));
    ComWidget::updateButtonState(ui->connectBtn, false);
    setControlsEnabled(true);
}

void tcpclientWidget::setControlsEnabled(bool on) {
    ui->ipEdit->setEnabled(on);
    ui->portEdit->setEnabled(on);
}
