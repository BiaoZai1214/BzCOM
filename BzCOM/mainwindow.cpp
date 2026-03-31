#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uartwidget.h"
#include "tcpwidget.h"
#include "otacontroller.h"
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>

//==============================================================================
// 构造 / 析构
//==============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_uartWidget(new uartWidget(this))
    , m_tcpWidget(new tcpWidget(this))
    , m_otaController(new OTAController(this))
    , m_isHexMode(false)
    , m_currentFilePath()
{
    ui->setupUi(this);
    initUi();
    initConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//==============================================================================
// 初始化
//==============================================================================

void MainWindow::initUi()
{
    setWindowTitle("标仔IAP助手");
    ui->textBrowser->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->checkBox->setEditable(false);
    ui->checkBox->setEditText("校验和 (0x00)");
    ui->hexBtn->setText("ASCII");
    ui->tabWidget->insertTab(0, m_uartWidget, tr("串口"));
    ui->tabWidget->insertTab(1, m_tcpWidget,  tr("TCP"));
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::initConnections()
{
    // UI 按钮
    connect(ui->clearLog,   &QPushButton::clicked, this, &MainWindow::clearLog);
    connect(ui->saveLog,    &QPushButton::clicked, this, &MainWindow::saveLog);
    connect(ui->sendBtn,    &QPushButton::clicked, this, &MainWindow::sendText);
    connect(ui->hexBtn,     &QPushButton::clicked, this, &MainWindow::toggleHexMode);
    connect(ui->selectFile,  &QPushButton::clicked, this, &MainWindow::selectFile);
    connect(ui->fileBtn,    &QPushButton::clicked, this, &MainWindow::onFileBtnClicked);
    connect(ui->textEdit,   &QTextEdit::textChanged, this, &MainWindow::updateChecksum);

    // 通信组件 → MainWindow（显示数据）
    connect(m_uartWidget, &uartWidget::dataReceived, this, &MainWindow::showReceivedData);
    connect(m_tcpWidget,  &tcpWidget::dataReceived, this, &MainWindow::showReceivedData);
    connect(m_uartWidget, &uartWidget::systemMessage, this, &MainWindow::showSystemMessage);
    connect(m_tcpWidget,  &tcpWidget::systemMessage, this, &MainWindow::showSystemMessage);

    // 通信组件 → OTA Controller（处理 OTA 数据）
    connect(m_uartWidget, &uartWidget::dataReceived, m_otaController, &OTAController::onDataReceived);
    connect(m_tcpWidget,  &tcpWidget::dataReceived, m_otaController, &OTAController::onDataReceived);

    // OTA Controller → MainWindow（显示 OTA 进度和结果）
    connect(m_otaController, &OTAController::sendData, this, &MainWindow::onOtaSendData);
    connect(m_otaController, &OTAController::progress, this, &MainWindow::onOtaProgress);
    connect(m_otaController, &OTAController::otaFinished, this, &MainWindow::onOtaFinished);
}

//==============================================================================
// UI 操作
//==============================================================================

void MainWindow::toggleHexMode()
{
    m_isHexMode = !m_isHexMode;
    ui->hexBtn->setText(m_isHexMode ? "HEX" : "ASCII");
    ui->checkBox->setCurrentIndex(m_isHexMode ? 1 : 0);
    updateChecksum();
}

void MainWindow::updateChecksum()
{
    if (!m_isHexMode) {
        ui->checkBox->setItemText(1, "校验和 (0x00)");
        return;
    }
    QByteArray data = QByteArray::fromHex(cleanHex(ui->textEdit->toPlainText()).toUtf8());
    if (data.isEmpty()) {
        ui->checkBox->setItemText(1, "校验和 (0x00)");
    } else {
        ui->checkBox->setItemText(1, QString("校验和 (0x%1)")
            .arg(calcChecksum(data), 2, 16, QChar('0')).toUpper());
    }
}

void MainWindow::selectFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("选择 IAP 升级文件"), QDir::homePath(),
        tr("BIN 文件 (*.bin);;HEX 文件 (*.hex);;所有文件 (*.*)"));
    if (path.isEmpty()) return;

    m_currentFilePath = path;
    QFileInfo fi(path);
    ui->selectFile->setText(fi.fileName());
    appendLog(QString("已选择文件：%1（大小：%2 字节）").arg(fi.fileName()).arg(fi.size()), SystemMsg, "#0066CC");
}

void MainWindow::clearLog()
{
    ui->textBrowser->clear();
}

void MainWindow::saveLog()
{
    QString log = ui->textBrowser->toPlainText().trimmed();
    if (log.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("日志内容为空，无需保存！"));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, tr("保存日志"),
        QString("日志_%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        tr("日志文件 (*.log *.txt);;所有文件 (*.*)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream(&file) << log;
        file.close();
        QMessageBox::information(this, tr("成功"), tr("日志已保存至：\n") + fileName);
    } else {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件：") + file.errorString());
    }
}

//==============================================================================
// 文件发送
//==============================================================================

void MainWindow::onFileBtnClicked()
{
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择文件！");
        return;
    }
    if (!isConnectionReady()) {
        QMessageBox::warning(this, "提示", "连接未就绪，无法发送文件！");
        return;
    }

    // 清空串口残留数据
    m_uartWidget->readAll();

    // 根据协议模式启动 OTA
    OTAController::CheckMode mode = (ui->ProtocolBox->currentText().trimmed() == "CRC16-2-2")
                                         ? OTAController::CHECK_MODBUS
                                         : OTAController::CHECK_NONE;
    m_otaController->startUpdate(m_currentFilePath, mode);
}

void MainWindow::onOtaSendData(const QByteArray &data)
{
    sendDataToActive(data);
}

void MainWindow::onOtaProgress(const QString &msg, int type)
{
    QString color;
    switch (type) {
        case 0: color = "#0066CC"; break;   // 系统消息
        case 1: color = "blue"; break;     // 发送中
        case 2: color = "#FF0000"; break;  // 失败
        default: color = "#0066CC";
    }
    appendLog(msg, SystemMsg, color);
}

void MainWindow::onOtaFinished(bool success, const QString &msg)
{
    appendLog(success ? ("✓ " + msg) : ("❌ " + msg),
              SystemMsg, success ? "#009900" : "#FF0000");
}

//==============================================================================
// 文本发送
//==============================================================================

void MainWindow::sendText()
{
    QString input = ui->textEdit->toPlainText().trimmed();
    if (input.isEmpty() || !isConnectionReady()) return;

    QByteArray rawData;
    QString display;

    if (m_isHexMode) {
        rawData = QByteArray::fromHex(cleanHex(input).toUtf8());
        if (rawData.isEmpty()) {
            QMessageBox::warning(this, "提示", "HEX 格式无效，请检查输入！");
            return;
        }
        rawData.append(static_cast<char>(calcChecksum(rawData)));
        QStringList hexList;
        for (char b : rawData)
            hexList << QString("0x%1").arg(static_cast<quint8>(b), 2, 16, QChar('0')).toUpper();
        display = hexList.join(", ");
    } else {
        display = input;
        rawData = input.toUtf8();
    }

    sendDataToActive(rawData);
    appendLog(display, Sent);
}

//==============================================================================
// 日志显示
//==============================================================================

void MainWindow::showReceivedData(const QByteArray &data)
{
    if (data.isEmpty()) return;
    QString display = formatData(data, m_isHexMode);
    if (!display.isEmpty()) appendLog(display, Received);
}

void MainWindow::showSystemMessage(const QString &msg)
{
    appendLog(msg, SystemMsg, "#0066CC");
}

void MainWindow::appendLog(const QString &text, LogType type, const QString &color)
{
    static const char *aligns[] = {"right", "left", "center"};
    int i = static_cast<int>(type);
    QString ts   = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString mode = m_isHexMode ? "hex" : "ascii";
    QString safe = text.toHtmlEscaped().replace("\n", "<br/>");

    QString html;
    if (type == SystemMsg) {
        html = QString("<table width='100%'><tr><td align='center'>"
                       "<font size='2' color='%1'>%2</font></td></tr></table>").arg(color, safe);
    } else {
        html = QString("<table width='100%'><tr><td align='%1'>"
                       "<font size='2' color='#666666'>%2 %3</font></td></tr>"
                       "<tr><td align='%1'><font face='Consolas'>%4</font></td></tr></table>")
                       .arg(aligns[i], ts, mode, safe);
    }
    ui->textBrowser->append(html);
    ui->textBrowser->moveCursor(QTextCursor::End);
}

//==============================================================================
// 辅助函数
//==============================================================================

quint8 MainWindow::calcChecksum(const QByteArray &data)
{
    quint16 sum = 0;
    for (char b : data) sum += static_cast<quint8>(b);
    return static_cast<quint8>(sum);
}

QString MainWindow::cleanHex(const QString &text)
{
    return QString(text).remove(' ').remove(',').remove("0x", Qt::CaseInsensitive).remove("0X");
}

QString MainWindow::formatData(const QByteArray &data, bool hex)
{
    if (hex) {
        QStringList list;
        list.reserve(data.size());
        for (char b : data)
            list << QString("0x%1").arg(static_cast<quint8>(b), 2, 16, QChar('0')).toUpper();
        return list.join(", ");
    }
    return QString::fromUtf8(data);
}

void MainWindow::sendDataToActive(const QByteArray &data)
{
    if (ui->tabWidget->currentIndex() == 0)
        m_uartWidget->sendData(data);
    else
        m_tcpWidget->sendData(data);
}

bool MainWindow::isConnectionReady() const
{
    return ui->tabWidget->currentIndex() == 0
               ? m_uartWidget->isSerialConnected()
               : m_tcpWidget->isConnected();
}
