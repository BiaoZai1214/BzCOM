#include "tcpserverwidget.h"
#include "ui_tcpserverwidget.h"
#include <QMessageBox>

tcpserverWidget::tcpserverWidget(QWidget *parent)
    : ComWidget(parent)
    , ui(new Ui::tcpserverWidget)
    , m_server(new QTcpServer(this))
{
    ui->setupUi(this);
    ui->listenBtn->setProperty("connected", false);

    connect(ui->listenBtn, &QPushButton::clicked, this, [this]() {
        if (m_server->isListening()) {
            m_server->close();
            zapSocket(m_client);
            ui->ipEdit->setEnabled(true);
            ui->portEdit->setEnabled(true);
            ui->listenBtn->setText(tr("侦听"));
            ComWidget::updateButtonState(ui->listenBtn, false);
            emit systemMessage(tr("服务端已停止"));
            return;
        }

        QString ip = ui->ipEdit->text().trimmed();
        quint16 port = ui->portEdit->text().toUShort();
        if (port == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请输入端口"));
            return;
        }
        QHostAddress addr(ip);
        if (ip.isEmpty() || addr.isNull()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入有效的IP地址"));
            return;
        }
        if (!m_server->listen(addr, port)) {
            QMessageBox::critical(this, tr("错误"), tr("侦听失败：%1").arg(m_server->errorString()));
            return;
        }
        ui->ipEdit->setEnabled(false);
        ui->portEdit->setEnabled(false);
        ui->listenBtn->setText(tr("停止侦听"));
        ComWidget::updateButtonState(ui->listenBtn, true);
        emit systemMessage(tr("服务端已启动：%1:%2").arg(ip).arg(port));
    });

    connect(m_server, &QTcpServer::newConnection, this, &tcpserverWidget::onNewClient);
}

tcpserverWidget::~tcpserverWidget()
{
    if (m_server->isListening()) m_server->close();
    zapSocket(m_client);
    delete ui;
}

bool tcpserverWidget::isConnected(void) const
{
    return m_client && m_client->state() == QAbstractSocket::ConnectedState;
}

void tcpserverWidget::sendData(const QByteArray &data)
{
    if (!isConnected()) { QMessageBox::warning(this, tr("提示"), tr("连接未就绪")); return; }
    qint64 written = m_client->write(data);
    if (written < 0) emit systemMessage(tr("[错误] 发送失败：%1").arg(m_client->errorString()));
}

void tcpserverWidget::disconnectDevice() {
    if (m_server->isListening()) m_server->close();
    zapSocket(m_client);
    ui->listenBtn->setText(tr("侦听"));
    ComWidget::updateButtonState(ui->listenBtn, false);
    ui->ipEdit->setEnabled(true);
    ui->portEdit->setEnabled(true);
}

void tcpserverWidget::onNewClient(void)
{
    QTcpSocket *sock = m_server->nextPendingConnection();
    if (!sock) return;
    zapSocket(m_client);
    m_client = sock;
    connect(m_client, &QTcpSocket::readyRead, this, [this]() {
        QByteArray d = m_client->readAll();
        if (!d.isEmpty()) emit dataReceived(d);
    });
    connect(m_client, &QTcpSocket::disconnected, this, &tcpserverWidget::onClientLeft);
    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &tcpserverWidget::onClientError);
    emit systemMessage(tr("客户端接入：%1:%2")
        .arg(m_client->peerAddress().toString()).arg(m_client->peerPort()));
}

void tcpserverWidget::onClientLeft(void)
{
    emit systemMessage(tr("客户端已断开"));
    zapSocket(m_client);
}

void tcpserverWidget::onClientError(QAbstractSocket::SocketError)
{
    if (!m_client) return;
    emit systemMessage(tr("[错误] %1").arg(m_client->errorString()));
    zapSocket(m_client);
}
