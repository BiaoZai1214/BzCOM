#include "canwidget.h"
#include "ui_canwidget.h"
#include <QMessageBox>
static const quint16 PCAN_USBBUS1    = 0x51;
static const quint16 PCAN_TYPE_USB   = 0x05;

static const quint32 PCAN_ERROR_OK         = 0x00000;
static const quint32 PCAN_ERROR_OVERRUN    = 0x00002;
static const quint32 PCAN_ERROR_QRCVEMPTY  = 0x00020;

#pragma pack(push, 1)
struct PCanMsg {
    quint32 id;
    quint8  msgType;
    quint8  len;
    quint8  data[8];
};
#pragma pack(pop)

canWidget::canWidget(QWidget *parent)
    : ComWidget(parent)
    , m_canInit(nullptr)
    , m_canRead(nullptr)
    , m_canSetValue(nullptr)
    , m_canUninit(nullptr)
    , m_loaded(false)
    , m_connected(false)
    , m_channel(PCAN_USBBUS1)
    , m_baudRate(0x001C)
    , ui(new Ui::canWidget)
    , m_pollTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->channelCombo->setStyleSheet(
        ui->channelCombo->styleSheet() + "QComboBox { font-size: 14px; }");
    ui->channelCombo->addItem("PCAN-USB 1", PCAN_USBBUS1);
    ui->channelCombo->addItem("PCAN-USB 2", PCAN_USBBUS1 + 1);
    ui->channelCombo->addItem("PCAN-USB 3", PCAN_USBBUS1 + 2);
    ui->channelCombo->addItem("PCAN-USB 4", PCAN_USBBUS1 + 3);

    ui->baudCombo->addItem("1 Mbit/s",   0x0014);
    ui->baudCombo->addItem("800 kbit/s", 0x0016);
    ui->baudCombo->addItem("500 kbit/s", 0x001C);
    ui->baudCombo->addItem("250 kbit/s", 0x011C);
    ui->baudCombo->addItem("125 kbit/s", 0x031C);
    ui->baudCombo->addItem("100 kbit/s", 0x042F);
    ui->baudCombo->addItem("50 kbit/s",  0x047F);
    ui->baudCombo->addItem("20 kbit/s",  0x0AFF);
    ui->baudCombo->setCurrentIndex(2);

    ui->filterCombo->addItem("全部帧ID", 0);
    ui->filterCombo->addItem("标准帧ID", 1);
    ui->filterCombo->addItem("扩展帧ID", 2);

    ui->modeCombo->addItem("正常模式", 0);
    ui->modeCombo->addItem("只听模式", 1);

    if (!loadPcanBasic())
        ui->connectBtn->setEnabled(false);

    connect(ui->connectBtn, &QPushButton::clicked, this, &canWidget::onConnectClicked);
    connect(m_pollTimer, &QTimer::timeout, this, &canWidget::onPollTimer);
}

canWidget::~canWidget()
{
    doDisconnect();
    if (m_pcanLib.isLoaded())
        m_pcanLib.unload();
    delete ui;
}

bool canWidget::loadPcanBasic(void)
{
    m_pcanLib.setFileName("PCANBasic");
    if (m_pcanLib.load()) {
        m_canInit     = reinterpret_cast<CAN_InitFunc>(m_pcanLib.resolve("CAN_Initialize"));
        m_canRead     = reinterpret_cast<CAN_ReadFunc>(m_pcanLib.resolve("CAN_Read"));
        m_canSetValue = reinterpret_cast<CAN_SetValueFunc>(m_pcanLib.resolve("CAN_SetValue"));
        m_canUninit   = reinterpret_cast<CAN_UninitFunc>(m_pcanLib.resolve("CAN_Uninitialize"));
        if (m_canInit && m_canRead && m_canUninit) { m_loaded = true; return true; }
        m_pcanLib.unload();
    }
    m_pcanLib.setFileName("PCANBasic_x64");
    if (m_pcanLib.load()) {
        m_canInit     = reinterpret_cast<CAN_InitFunc>(m_pcanLib.resolve("CAN_Initialize"));
        m_canRead     = reinterpret_cast<CAN_ReadFunc>(m_pcanLib.resolve("CAN_Read"));
        m_canSetValue = reinterpret_cast<CAN_SetValueFunc>(m_pcanLib.resolve("CAN_SetValue"));
        m_canUninit   = reinterpret_cast<CAN_UninitFunc>(m_pcanLib.resolve("CAN_Uninitialize"));
        if (m_canInit && m_canRead && m_canUninit) { m_loaded = true; return true; }
        m_pcanLib.unload();
    }
    emit systemMessage(tr("❌ 加载 PCANBasic.dll 失败，请安装 PEAK-System 驱动"));
    return false;
}

bool canWidget::isConnected(void) const { return m_connected; }

void canWidget::onConnectClicked(void)
{
    m_connected ? doDisconnect() : doConnect();
}

void canWidget::doConnect(void)
{
    if (!m_loaded) { QMessageBox::warning(this, tr("提示"), tr("PCANBasic 驱动未加载")); return; }
    m_channel  = static_cast<quint16>(ui->channelCombo->currentData().toUInt());
    m_baudRate = static_cast<quint16>(ui->baudCombo->currentData().toUInt());
    bool listenOnly = (ui->modeCombo->currentData().toInt() == 1);
    doDisconnect();

    quint32 status = m_canInit(m_channel, m_baudRate, PCAN_TYPE_USB, 0, 0);
    if (status != PCAN_ERROR_OK) {
        emit systemMessage(tr("❌ CAN 初始化失败 (0x%1)").arg(status, 4, 16, QChar('0')));
        return;
    }

    if (listenOnly && m_canSetValue) {
        quint8 val = 1;
        m_canSetValue(m_channel, 0x06, &val, 1);
    }

    m_connected = true;
    ui->connectBtn->setText(tr("断开"));
    ComWidget::updateButtonState(ui->connectBtn, true);
    ui->channelCombo->setEnabled(false);
    ui->baudCombo->setEnabled(false);
    ui->filterCombo->setEnabled(false);
    ui->modeCombo->setEnabled(false);
    m_pollTimer->start(100);
    emit systemMessage(tr("✅ CAN 已连接 - %1, 通道 0x%2")
        .arg(ui->baudCombo->currentText()).arg(m_channel, 4, 16, QChar('0')).toUpper());
}

void canWidget::doDisconnect(void)
{
    if (!m_connected) return;
    m_pollTimer->stop();
    if (m_loaded) m_canUninit(m_channel);
    m_connected = false;
    ui->connectBtn->setText(tr("连接"));
    ComWidget::updateButtonState(ui->connectBtn, false);
    ui->channelCombo->setEnabled(true);
    ui->baudCombo->setEnabled(true);
    ui->filterCombo->setEnabled(true);
    ui->modeCombo->setEnabled(true);
    emit systemMessage(tr("🔌 CAN 已断开"));
}

void canWidget::onPollTimer(void)
{
    if (!m_connected || !m_loaded) return;

    PCanMsg msg;
    bool overflow = false;
    int filter = ui->filterCombo->currentData().toInt();

    for (int i = 0; i < 50; i++) {
        quint32 status = m_canRead(m_channel, &msg, nullptr);
        if (status == PCAN_ERROR_QRCVEMPTY) break;
        if (status == PCAN_ERROR_OVERRUN)  { overflow = true; continue; }
        if (status != PCAN_ERROR_OK) continue;

        bool isExt = (msg.msgType & 0x02);
        if ((filter == 1 && isExt) || (filter == 2 && !isExt))
            continue;

        emit dataReceived(
            formatFrame(msg.id, msg.msgType, msg.data, msg.len).toUtf8());
    }

    if (overflow) emit systemMessage(tr("⚠️ CAN 接收溢出，部分帧已丢失"));
}

QString canWidget::formatFrame(quint32 id, quint8 msgType,
                                          const quint8 *data, quint8 len)
{
    QString typeFlag = (msgType & 0x02) ? "EXT" : "STD";
    QString rtrFlag  = (msgType & 0x01) ? " RTR" : QString();

    QString idStr;
    if (msgType & 0x02)
        idStr = QString::number(id, 16).toUpper().rightJustified(8, '0');
    else
        idStr = QString::number(id, 16).toUpper().rightJustified(3, '0');

    QString s = QString("%1 0x%2 [%3]%4")
                    .arg(typeFlag).arg(idStr).arg(len).arg(rtrFlag);

    if (!(msgType & 0x01)) {
        for (int i = 0; i < len && i < 8; i++)
            s += QString(" %1").arg(data[i], 2, 16, QChar('0')).toUpper();
    }
    return s;
}
