#include "tcpwidget.h"
#include "ui_tcpwidget.h"
#include <QMessageBox>
#include <QHostAddress>

tcpWidget::tcpWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::tcpWidget)
    , m_tcpServer(new QTcpServer(this))
    , m_tcpSocket(nullptr)
    , m_clientSocket(nullptr)
    , m_udpSocket(new QUdpSocket(this))
    , m_currentMode(TcpClientMode)
    , m_isListening(false)
    , m_isConnected(false)
    , m_isUdpBound(false)
{
    ui->setupUi(this);

    for (const QHostAddress &addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost) {
            ui->ipEdit2->setText(addr.toString());
            break;
        }
    }

    connect(ui->tcpModeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &tcpWidget::onModeChanged);
    connect(ui->connectBtn, &QPushButton::clicked,  this, &tcpWidget::onBtnClicked);
    connect(m_tcpServer, &QTcpServer::newConnection,this, &tcpWidget::onNewClient);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &tcpWidget::onUdpReadyRead);

    refreshUI();
}

tcpWidget::~tcpWidget() { reset(); delete ui; }

bool tcpWidget::isConnected() const
{
    if (m_currentMode == UdpMode) return m_isUdpBound;
    if (m_currentMode == TcpClientMode) return m_isConnected && m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState;
    return m_isConnected && m_clientSocket && m_clientSocket->state() == QAbstractSocket::ConnectedState;
}

void tcpWidget::sendData(const QByteArray &data)
{
    if (m_currentMode == UdpMode) {
        if (!m_isUdpBound) { QMessageBox::warning(this, "提示", "UDP未绑定"); return; }
        QString ip = ui->ipEdit1->text().trimmed();
        quint16 port = ui->portEdit1->text().trimmed().toUShort();
        if (ip.isEmpty() || port == 0) { QMessageBox::warning(this, "提示", "请输入远程IP和端口"); return; }
        m_udpSocket->writeDatagram(data, QHostAddress(ip), port);
        return;
    }

    QTcpSocket *sock = (m_currentMode == TcpClientMode) ? m_tcpSocket : m_clientSocket;
    if (!sock || sock->state() != QAbstractSocket::ConnectedState) { QMessageBox::warning(this, "提示", "连接未就绪"); return; }
    sock->write(data);
}

void tcpWidget::onModeChanged(int idx)
{
    reset();
    QString text = ui->tcpModeBox->itemText(idx);
    if (text == "TCPClient") m_currentMode = TcpClientMode;
    else if (text == "TCPServer") m_currentMode = TcpServerMode;
    else m_currentMode = UdpMode;
    refreshUI();
}

void tcpWidget::onBtnClicked()
{
    switch (m_currentMode) {
    case TcpClientMode: m_isConnected ? doDisconnect() : doConnect(); break;
    case TcpServerMode: m_isListening ? doStopListen() : doListen();  break;
    case UdpMode: m_isUdpBound ? doUdpUnbind() : doUdpBind(); break;
    }
}

void tcpWidget::doConnect()
{
    QString ip = ui->ipEdit1->text().trimmed();
    quint16 port = ui->portEdit1->text().trimmed().toUShort();
    if (ip.isEmpty()) {QMessageBox::warning(this, "提示", "请输入服务器IP"); return; }
    if (port == 0)    {QMessageBox::warning(this, "提示", "请输入端口"); return; }

    if (m_tcpSocket) { m_tcpSocket->abort(); delete m_tcpSocket; }
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &tcpWidget::onConnected);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &tcpWidget::onReadyRead);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &tcpWidget::onDisconnected);
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &tcpWidget::onError);

    m_tcpSocket->connectToHost(ip, port);
    ui->connectBtn->setText("连接中...");
    ui->connectBtn->setEnabled(false);
}

void tcpWidget::doDisconnect()
{
    if (m_tcpSocket) { m_tcpSocket->abort(); delete m_tcpSocket; m_tcpSocket = nullptr; }
    m_isConnected = false;
    refreshUI();
}

void tcpWidget::doListen()
{
    QString ip = ui->ipEdit2->text().trimmed();
    quint16 port = ui->portEdit2->text().trimmed().toUShort();
    QHostAddress addr = ip.isEmpty() ? QHostAddress::Any : QHostAddress(ip);
    if (port == 0) { QMessageBox::warning(this, "提示", "请输入端口"); return; }

    if (!m_tcpServer->listen(addr, port)) {
        QMessageBox::critical(this, "错误", QString("侦听失败：%1").arg(m_tcpServer->errorString()));
        return;
    }
    m_isListening = true;
    ui->connectBtn->setText("停止侦听");
    emit systemMessage(QString("服务端已启动：%1:%2").arg(addr.toString()).arg(port));
    refreshUI();
}

void tcpWidget::doStopListen()
{
    reset();
    emit systemMessage("服务端已停止");
    refreshUI();
}

void tcpWidget::doUdpBind()
{
    QString ip = ui->ipEdit2->text().trimmed();
    quint16 port = ui->portEdit2->text().trimmed().toUShort();
    QHostAddress addr = ip.isEmpty() ? QHostAddress::Any : QHostAddress(ip);
    if (port == 0) { QMessageBox::warning(this, "提示", "请输入端口"); return; }

    if (!m_udpSocket->bind(addr, port)) {
        QMessageBox::critical(this, "错误", QString("UDP绑定失败：%1").arg(m_udpSocket->errorString()));
        return;
    }
    m_isUdpBound = true;
    ui->connectBtn->setText("解绑");
    emit systemMessage(QString("UDP已绑定：%1:%2").arg(addr.toString()).arg(port));
    refreshUI();
}

void tcpWidget::doUdpUnbind()
{
    m_udpSocket->close();
    m_isUdpBound = false;
    ui->connectBtn->setText("绑定");
    emit systemMessage("UDP已解绑");
    refreshUI();
}

void tcpWidget::onNewClient()
{
    if (m_clientSocket) { m_clientSocket->abort(); delete m_clientSocket; }
    m_clientSocket = m_tcpServer->nextPendingConnection();
    if (!m_clientSocket) return;

    connect(m_clientSocket, &QTcpSocket::readyRead, this, &tcpWidget::onClientReadyRead);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &tcpWidget::onClientLeft);
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &tcpWidget::onClientError);

    m_isConnected = true;
    emit connectionStateChanged(true);
    emit systemMessage(QString("客户端接入：%1:%2").arg(m_clientSocket->peerAddress().toString()).arg(m_clientSocket->peerPort()));
    refreshUI();
}

void tcpWidget::onClientLeft()
{
    m_isConnected = false;
    emit connectionStateChanged(false);
    emit systemMessage("客户端已断开");
    if (m_clientSocket) { delete m_clientSocket; m_clientSocket = nullptr; }
    refreshUI();
}

void tcpWidget::onClientReadyRead()
{
    if (!m_clientSocket) return;
    QByteArray data = m_clientSocket->readAll();
    if (!data.isEmpty()) emit dataReceived(data);
}

void tcpWidget::onClientError(QAbstractSocket::SocketError)
{
    if (!m_clientSocket) return;
    emit systemMessage(QString("[错误] %1").arg(m_clientSocket->errorString()));
    m_isConnected = false;
    refreshUI();
}

void tcpWidget::onConnected()
{
    m_isConnected = true;
    ui->connectBtn->setEnabled(true);
    emit connectionStateChanged(true);
    emit systemMessage(QString("已连接：%1:%2").arg(m_tcpSocket->peerAddress().toString()).arg(m_tcpSocket->peerPort()));
    refreshUI();
}

void tcpWidget::onReadyRead()
{
    if (!m_tcpSocket) return;
    QByteArray data = m_tcpSocket->readAll();
    if (!data.isEmpty()) emit dataReceived(data);
}

void tcpWidget::onDisconnected()
{
    m_isConnected = false;
    ui->connectBtn->setEnabled(true);
    emit connectionStateChanged(false);
    emit systemMessage("已断开连接");
    refreshUI();
}

void tcpWidget::onError(QAbstractSocket::SocketError)
{
    if (!m_tcpSocket) return;
    emit systemMessage(QString("[错误] %1").arg(m_tcpSocket->errorString()));
    m_isConnected = false;
    ui->connectBtn->setEnabled(true);
    refreshUI();
}

void tcpWidget::onUdpReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray data(m_udpSocket->pendingDatagramSize(), 0);
        QHostAddress sender;
        quint16 senderPort;
        m_udpSocket->readDatagram(data.data(), data.size(), &sender, &senderPort);
        if (!data.isEmpty()) emit dataReceived(data);
    }
}

void tcpWidget::reset()
{
    if (m_tcpSocket) { m_tcpSocket->abort(); delete m_tcpSocket; m_tcpSocket = nullptr; }
    if (m_clientSocket) { m_clientSocket->abort(); delete m_clientSocket; m_clientSocket = nullptr; }
    if (m_tcpServer && m_tcpServer->isListening()) m_tcpServer->close();
    if (m_isUdpBound) { m_udpSocket->close(); m_isUdpBound = false; }
    m_isListening = false;
    m_isConnected = false;
}

void tcpWidget::refreshUI()
{
    bool busy = (m_currentMode == TcpClientMode && m_isConnected)
             || (m_currentMode == TcpServerMode && m_isListening)
             || (m_currentMode == UdpMode && m_isUdpBound);

    /* 按钮文字 */
    if (m_currentMode == TcpClientMode && m_isConnected)
            ui->connectBtn->setText("断开连接");
    else if (m_currentMode == TcpServerMode && m_isListening)
            ui->connectBtn->setText("停止侦听");
    else if (m_currentMode == UdpMode && m_isUdpBound)
            ui->connectBtn->setText("解绑");
    else { static const char* texts[] = {"连接", "侦听", "绑定"};
            ui->connectBtn->setText(texts[m_currentMode]); }

    ui->tcpModeBox->setEnabled(!busy);
    ui->ipEdit1->setEnabled(!busy && (m_currentMode == TcpClientMode || m_currentMode == UdpMode));
    ui->portEdit1->setEnabled(!busy && (m_currentMode == TcpClientMode || m_currentMode == UdpMode));
    ui->ipEdit2->setEnabled(!busy);
    ui->portEdit2->setEnabled(!busy);

    ui->connectBtn->setProperty("connected", busy);
    ui->connectBtn->style()->unpolish(ui->connectBtn);
    ui->connectBtn->style()->polish(ui->connectBtn);
}
