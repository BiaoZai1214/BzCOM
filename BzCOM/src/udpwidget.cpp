#include "udpwidget.h"
#include "ui_udpwidget.h"
#include <QPushButton>
#include <QMessageBox>

udpWidget::udpWidget(QWidget *parent)
    : ComWidget(parent)
    , ui(new Ui::udpWidget)
    , m_socket(new QUdpSocket(this))
{
    ui->setupUi(this);
    ui->bindBtn->setProperty("connected", false);

    connect(m_socket, &QUdpSocket::readyRead, this, [this]() {
        while (m_socket->hasPendingDatagrams()) {
            QByteArray d(m_socket->pendingDatagramSize(), 0);
            QHostAddress sender; quint16 port;
            m_socket->readDatagram(d.data(), d.size(), &sender, &port);
            if (!d.isEmpty()) emit dataReceived(d);
        }
    });

    connect(ui->bindBtn, &QPushButton::clicked, this, [this]() {
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->close();
            ui->bindBtn->setText(tr("绑定"));
            ComWidget::updateButtonState(ui->bindBtn, false);
            ui->remoteIpEdit->setEnabled(true);
            ui->remotePortEdit->setEnabled(true);
            ui->localPortEdit->setEnabled(true);
            emit systemMessage(tr("UDP已解绑"));
            return;
        }
        m_localPort = ui->localPortEdit->text().toUShort();
        m_remoteHost = ui->remoteIpEdit->text().trimmed();
        m_remotePort = ui->remotePortEdit->text().toUShort();
        if (m_localPort == 0 || m_remotePort == 0 || m_remoteHost.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请填写完整参数"));
            return;
        }
        if (!m_socket->bind(QHostAddress::AnyIPv4, m_localPort)) {
            QMessageBox::critical(this, tr("错误"), tr("UDP绑定失败"));
            return;
        }
        ui->bindBtn->setText(tr("解绑"));
        ComWidget::updateButtonState(ui->bindBtn, true);
        ui->remoteIpEdit->setEnabled(false);
        ui->remotePortEdit->setEnabled(false);
        ui->localPortEdit->setEnabled(false);
        emit systemMessage(tr("UDP已绑定：本地%1").arg(m_localPort));
    });
}

void udpWidget::disconnectDevice() {
    m_socket->close();
    ui->bindBtn->setText(tr("绑定"));
    ComWidget::updateButtonState(ui->bindBtn, false);
    ui->remoteIpEdit->setEnabled(true);
    ui->remotePortEdit->setEnabled(true);
    ui->localPortEdit->setEnabled(true);
}

udpWidget::~udpWidget() {
    m_socket->close();
    delete ui;
}

bool udpWidget::isConnected(void) const {
    return m_socket->state() != QAbstractSocket::UnconnectedState;
}

void udpWidget::sendData(const QByteArray &data) {
    if (!isConnected()) { QMessageBox::warning(this, tr("提示"), tr("UDP未绑定")); return; }
    if (m_remoteHost.isEmpty() || m_remotePort == 0) {
        QMessageBox::warning(this, tr("提示"), tr("请输入目标IP和端口"));
        return;
    }
    qint64 written = m_socket->writeDatagram(data, QHostAddress(m_remoteHost), m_remotePort);
    if (written < 0) emit systemMessage(tr("[错误] UDP发送失败：%1").arg(m_socket->errorString()));
}
