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

    m_widgets[0] = new tcpclientWidget(this);
    m_widgets[1] = new tcpserverWidget(this);
    m_widgets[2] = new udpWidget(this);
    for (int i = 0; i < 3; i++) {
        ui->stackedWidget->addWidget(m_widgets[i]);
        routeSignal(m_widgets[i]);
    }
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &tcpWidget::onModeChanged);
}

tcpWidget::~tcpWidget() {
    delete ui;
}

void tcpWidget::routeSignal(ComWidget *w) {
    connect(w, &ComWidget::dataReceived, this, &tcpWidget::dataReceived);
    connect(w, &ComWidget::systemMessage, this, &tcpWidget::systemMessage);
}

void tcpWidget::onModeChanged(int idx) { ui->stackedWidget->setCurrentIndex(idx); }

bool tcpWidget::isConnected(void) const {
    return m_widgets[ui->stackedWidget->currentIndex()]->isConnected();
}

void tcpWidget::sendData(const QByteArray &data) {
    m_widgets[ui->stackedWidget->currentIndex()]->sendData(data);
}
