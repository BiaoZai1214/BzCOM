#include "tcpwidget.h"
#include "ui_tcpwidget.h"
#include "tcpclientwidget.h"
#include "tcpserverwidget.h"
#include "udpwidget.h"

tcpWidget::tcpWidget(QWidget *parent)
    : ComWidget(parent)
    , ui(new Ui::tcpWidget)
{
    ui->setupUi(this);

    for (int i = 0; i < 3; i++) m_widgets[i] = nullptr;

    m_widgets[0] = new tcpclientWidget(this);
    m_widgets[1] = new tcpserverWidget(this);
    m_widgets[2] = new udpWidget(this);

    ui->stackedWidget->addWidget(m_widgets[0]);
    ui->stackedWidget->addWidget(m_widgets[1]);
    ui->stackedWidget->addWidget(m_widgets[2]);

    for (int i = 0; i < 3; i++) routeSignal(m_widgets[i]);

    connect(ui->modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &tcpWidget::onModeChanged);
}

tcpWidget::~tcpWidget() { delete ui; }

bool tcpWidget::isConnected(void) const
{
    return m_widgets[ui->stackedWidget->currentIndex()]->isConnected();
}

void tcpWidget::sendData(const QByteArray &data)
{
    m_widgets[ui->stackedWidget->currentIndex()]->sendData(data);
}

void tcpWidget::onModeChanged(int idx)
{
    // 切换模式前断开所有子widget的连接（尤其是正在连接中的TCP客户端）
    for (int i = 0; i < 3; i++) m_widgets[i]->disconnectDevice();
    ui->stackedWidget->setCurrentIndex(idx);
}

void tcpWidget::routeSignal(ComWidget *w)
{
    connect(w, &ComWidget::dataReceived, this, &ComWidget::dataReceived);
    connect(w, &ComWidget::systemMessage, this, &ComWidget::systemMessage);
}
