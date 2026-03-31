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
    initUI();
    initConnections();
}

tcpWidget::~tcpWidget() { reset(); delete ui; }

bool tcpWidget::isConnected() const
{
    if (m_currentMode == UdpMode)
        return m_isUdpBound;
    if (m_currentMode == TcpClientMode)
        return m_isConnected && m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState;
    return m_isConnected && m_clientSocket && m_clientSocket->state() == QAbstractSocket::ConnectedState;
}

void tcpWidget::sendData(const QByteArray &data)
{
    if (m_currentMode == UdpMode) {
        if (!m_isUdpBound) {
            QMessageBox::warning(this, "提示", "UDP未绑定，无法发送数据！");
            return;
        }
        QString ip   = ui->ipEdit1->text().trimmed();
        quint16 port = ui->portEdit1->text().trimmed().toUShort();
        if (ip.isEmpty() || port == 0) {
            QMessageBox::warning(this, "提示", "请输入远程IP和端口！");
            return;
        }
        m_udpSocket->writeDatagram(data, QHostAddress(ip), port);
        return;
    }

    QTcpSocket *sock = (m_currentMode == TcpClientMode) ? m_tcpSocket : m_clientSocket;
    if (!sock || sock->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "提示", "TCP连接未就绪，无法发送数据！");
        return;
    }
    sock->write(data);
    sock->flush();
}

// 初始化
void tcpWidget::initUI()
{
    // 自动填入本机第一个非回环 IPv4 地址
    QString localIp;
    for (const QHostAddress &addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol
            && addr != QHostAddress::LocalHost) {
            localIp = addr.toString();
            break;
        }
    }
    ui->ipEdit2->setText(localIp);
    ui->portEdit1->setText("9999");
    ui->portEdit2->setText("8888");
    refreshUI();
}

void tcpWidget::initConnections()
{
    connect(ui->tcpModeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &tcpWidget::onModeChanged);
    connect(ui->connectBtn, &QPushButton::clicked, this, &tcpWidget::onBtnClicked);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &tcpWidget::onNewClient);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &tcpWidget::onUdpReadyRead);
}

// 模式切换与按钮分发
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
    case TcpClientMode:
        m_isConnected ? doDisconnect() : doConnect();
        break;
    case TcpServerMode:
        m_isListening ? doStopListen() : doListen();
        break;
    case UdpMode:
        m_isUdpBound ? doUdpUnbind() : doUdpBind();
        break;
    }
}

// 客户端连接
void tcpWidget::doConnect()
{
    QString ip   = ui->ipEdit1->text().trimmed();
    quint16 port = ui->portEdit1->text().trimmed().toUShort();

    if (ip.isEmpty()) { QMessageBox::warning(this, "错误", "请输入远程服务器IP地址！"); return; }
    if (port == 0)    { QMessageBox::warning(this, "错误", "请输入有效端口号！");      return; }

    // 创建新的客户端socket
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
    }
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &tcpWidget::onConnected);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &tcpWidget::onReadyRead);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &tcpWidget::onDisconnected);
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &tcpWidget::onError);

    // 本地绑定（可选）
    QString localIp = ui->ipEdit2->text().trimmed();
    quint16 localPort = ui->portEdit2->text().trimmed().toUShort();
    if (!localIp.isEmpty() || localPort != 0) {
        QHostAddress addr = localIp.isEmpty() ? QHostAddress::Any : QHostAddress(localIp);
        m_tcpSocket->bind(addr, localPort);
    }

    m_tcpSocket->connectToHost(ip, port);
    ui->connectBtn->setText("连接中...");
    ui->connectBtn->setEnabled(false);
}

// 客户端断开
void tcpWidget::doDisconnect()
{
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
    }
}

// 服务端监听
void tcpWidget::doListen()
{
    QString ip   = ui->ipEdit2->text().trimmed();
    quint16 port = ui->portEdit2->text().trimmed().toUShort();

    QHostAddress addr = ip.isEmpty() ? QHostAddress::Any : QHostAddress(ip);
    if (port == 0) {
        QMessageBox::warning(this, "错误", "请输入有效端口号！");
        return;
    }

    if (!m_tcpServer->listen(addr, port)) {
        QMessageBox::critical(this, "错误", QString("侦听失败：%1").arg(m_tcpServer->errorString()));
        return;
    }

    m_isListening = true;
    ui->connectBtn->setText("停止侦听");
    emit systemMessage(QString("服务端已启动侦听，地址：%1:%2").arg(addr.toString()).arg(port));
    refreshUI();
}

void tcpWidget::doStopListen()
{
    reset();
    emit systemMessage("服务端已停止侦听");
    refreshUI();
}

// UDP 绑定
void tcpWidget::doUdpBind()
{
    QString ip   = ui->ipEdit2->text().trimmed();
    quint16 port = ui->portEdit2->text().trimmed().toUShort();

    QHostAddress addr = ip.isEmpty() ? QHostAddress::Any : QHostAddress(ip);
    if (port == 0) {
        QMessageBox::warning(this, "错误", "请输入有效端口号！");
        return;
    }

    if (!m_udpSocket->bind(addr, port)) {
        QMessageBox::critical(this, "错误", QString("UDP绑定失败：%1").arg(m_udpSocket->errorString()));
        return;
    }

    m_isUdpBound = true;
    ui->connectBtn->setText("解绑");
    emit systemMessage(QString("UDP已绑定，地址：%1:%2").arg(addr.toString()).arg(port));
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

// 服务端：新客户端连接
void tcpWidget::onNewClient()
{
    // 关闭之前的客户端连接
    if (m_clientSocket) {
        m_clientSocket->disconnectFromHost();
        m_clientSocket->deleteLater();
    }

    m_clientSocket = m_tcpServer->nextPendingConnection();
    if (!m_clientSocket) return;

    connect(m_clientSocket, &QTcpSocket::readyRead, this, &tcpWidget::onClientReadyRead);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &tcpWidget::onClientLeft);
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &tcpWidget::onClientError);

    m_isConnected = true;
    emit connectionStateChanged(true);
    emit systemMessage(QString("客户端已接入：%1:%2")
                           .arg(m_clientSocket->peerAddress().toString())
                           .arg(m_clientSocket->peerPort()));
    refreshUI();
}

void tcpWidget::onClientLeft()
{
    m_isConnected = false;
    emit connectionStateChanged(false);
    emit systemMessage("客户端已断开连接");
    if (m_clientSocket) {
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }
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
    QString err = m_clientSocket->errorString();
    QMessageBox::warning(this, "客户端错误", err);
    emit systemMessage(QString("[错误] %1").arg(err));
    m_isConnected = false;
    refreshUI();
}

// 客户端回调
void tcpWidget::onConnected()
{
    m_isConnected = true;
    ui->connectBtn->setEnabled(true);
    emit connectionStateChanged(true);
    emit systemMessage(QString("已成功连接服务器：%1:%2")
                           .arg(m_tcpSocket->peerAddress().toString())
                           .arg(m_tcpSocket->peerPort()));
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
    emit systemMessage("已断开与服务器的连接");
    refreshUI();
}

void tcpWidget::onError(QAbstractSocket::SocketError)
{
    if (!m_tcpSocket) return;
    QString err = m_tcpSocket->errorString();
    QMessageBox::warning(this, "TCP连接错误", err);
    emit systemMessage(QString("[错误] %1").arg(err));
    m_isConnected = false;
    ui->connectBtn->setEnabled(true);
    refreshUI();
}

// UDP 回调
void tcpWidget::onUdpReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray data(m_udpSocket->pendingDatagramSize(), 0);
        QHostAddress sender;
        quint16 senderPort;
        m_udpSocket->readDatagram(data.data(), data.size(), &sender, &senderPort);
        if (!data.isEmpty())
            emit dataReceived(data);
    }
}

// 重置所有状态
void tcpWidget::reset()
{
    // 清理客户端socket
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }

    // 清理服务端已连接客户端
    if (m_clientSocket) {
        m_clientSocket->disconnectFromHost();
        m_clientSocket->deleteLater();
        m_clientSocket = nullptr;
    }

    // 关闭服务器监听
    if (m_tcpServer && m_tcpServer->isListening()) {
        m_tcpServer->close();
    }

    // 关闭UDP
    if (m_isUdpBound) {
        m_udpSocket->close();
    }

    m_isListening = false;
    m_isConnected = false;
    m_isUdpBound = false;
}

void tcpWidget::refreshUI()
{
    // 按钮文字
    if (m_currentMode == TcpClientMode && m_isConnected) {
        ui->connectBtn->setText("断开连接");
    } else if (m_currentMode == TcpServerMode && m_isListening) {
        ui->connectBtn->setText("停止侦听");
    } else if (m_currentMode == UdpMode && m_isUdpBound) {
        ui->connectBtn->setText("解绑");
    } else {
        static const char* texts[] = {"连接", "侦听", "绑定"};
        ui->connectBtn->setText(texts[m_currentMode]);
    }

    bool cIp = (m_currentMode == TcpClientMode);
    bool sIp = (m_currentMode == TcpServerMode);
    bool udp = (m_currentMode == UdpMode);

    ui->ipEdit1   ->setEnabled(cIp || udp);
    ui->portEdit1 ->setEnabled(cIp || udp);
    ui->ipEdit2   ->setEnabled(cIp || sIp || udp);
    ui->portEdit2 ->setEnabled(cIp || sIp || udp);
    ui->connectBtn->setEnabled(true);

    // 根据状态锁定控件
    if (m_currentMode == TcpClientMode && m_isConnected) {
        ui->tcpModeBox->setEnabled(false);
        ui->ipEdit1->setEnabled(false);
        ui->portEdit1->setEnabled(false);
        ui->ipEdit2->setEnabled(false);
        ui->portEdit2->setEnabled(false);
    } else if (m_currentMode == TcpServerMode && m_isListening) {
        ui->tcpModeBox->setEnabled(false);
        ui->ipEdit2->setEnabled(false);
        ui->portEdit2->setEnabled(false);
    } else if (m_currentMode == UdpMode && m_isUdpBound) {
        ui->tcpModeBox->setEnabled(false);
        ui->ipEdit2->setEnabled(false);
        ui->portEdit2->setEnabled(false);
    } else {
        ui->tcpModeBox->setEnabled(true);
    }

    // 按钮样式
    bool connected = (m_currentMode == TcpClientMode && m_isConnected)
                 || (m_currentMode == TcpServerMode && m_isListening)
                 || (m_currentMode == UdpMode && m_isUdpBound);
    ui->connectBtn->setProperty("connected", connected);
    ui->connectBtn->style()->unpolish(ui->connectBtn);
    ui->connectBtn->style()->polish(ui->connectBtn);
}
