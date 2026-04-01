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

    setWindowTitle("BzCOM 标仔IAP调试工具 V1.0");

    ui->tabWidget->insertTab(0, m_uartWidget, tr("串口"));
    ui->tabWidget->insertTab(1, m_tcpWidget,  tr("TCP"));

    // # 主界面控件
    connect(ui->clearLog,  &QPushButton::clicked, this, &MainWindow::clearLog);
    connect(ui->saveLog,   &QPushButton::clicked, this, &MainWindow::saveLog);
    connect(ui->sendBtn,   &QPushButton::clicked, this, &MainWindow::sendText);
    connect(ui->hexBtn,    &QPushButton::clicked, this, &MainWindow::toggleHexMode);
    connect(ui->selectFile,&QPushButton::clicked, this, &MainWindow::selectFile);
    connect(ui->fileBtn,   &QPushButton::clicked, this, &MainWindow::onFileClicked);
    connect(ui->textEdit,  &QTextEdit::textChanged, this, &MainWindow::updateChecksum);
    // # TCP控件
    connect(m_uartWidget, &uartWidget::dataReceived, this, &MainWindow::showReceivedData);
    connect(m_tcpWidget,  &tcpWidget::dataReceived, this, &MainWindow::showReceivedData);
    connect(m_uartWidget, &uartWidget::systemMessage, this, &MainWindow::showSystemMessage);
    connect(m_tcpWidget,  &tcpWidget::systemMessage, this, &MainWindow::showSystemMessage);
    // # 串口控件
    connect(m_uartWidget,    &uartWidget::dataReceived, m_otaController, &OTAController::onDataReceived);
    connect(m_tcpWidget,     &tcpWidget::dataReceived,  m_otaController, &OTAController::onDataReceived);
    connect(m_otaController, &OTAController::sendData, this, &MainWindow::sendData);
    connect(m_otaController, &OTAController::progress, this, &MainWindow::onOtaProgress);
    connect(m_otaController, &OTAController::otaFinished, this, &MainWindow::onOtaFinished);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::toggleHexMode()
{
    m_isHexMode = !m_isHexMode;
    ui->hexBtn->setText(m_isHexMode ? "HEX" : "ASCII");
    ui->checkBox->setCurrentIndex(m_isHexMode ? 1 : 0);
    updateChecksum();
}

void MainWindow::updateChecksum()
{
    QString text = "校验和 (0x00)";
    if (m_isHexMode) {
        QByteArray data = QByteArray::fromHex(
            ui->textEdit->toPlainText().remove(' ').remove(',').remove("0x", Qt::CaseInsensitive).remove("0X").toUtf8());
        if (!data.isEmpty()) {
            quint16 sum = 0;
            for (char b : data) sum += static_cast<quint8>(b);
            text = QString("校验和 (0x%1)").arg(sum & 0xFF, 2, 16, QChar('0')).toUpper();
        }
    }
    ui->checkBox->setItemText(1, text);
}

void MainWindow::selectFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择文件"), QDir::homePath(),
        tr("BIN文件(*.bin);;HEX文件(*.hex);;所有文件(*.*)"));
    if (path.isEmpty()) return;

    m_currentFilePath = path;
    QFileInfo fi(path);
    ui->selectFile->setText(fi.fileName());
    appendLog(QString("已选择：%1（%2 字节）").arg(fi.fileName()).arg(fi.size()), SystemMsg, "#0066CC");
}

void MainWindow::clearLog() { ui->textBrowser->clear(); }

void MainWindow::saveLog()
{
    QString log = ui->textBrowser->toPlainText().trimmed();
    if (log.isEmpty()) { QMessageBox::information(this, "提示", "日志为空"); return; }
    QString fileName = QFileDialog::getSaveFileName(this, "保存日志",
        QString("日志_%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        tr("日志文件(*.log *.txt)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream(&file) << log;
        file.close();
        QMessageBox::information(this, "成功", "已保存");
    }
}

void MainWindow::onFileClicked()
{
    if (m_currentFilePath.isEmpty()) { QMessageBox::information(this, "提示", "请先选择文件"); return; }
    if (!isConnected()) { QMessageBox::warning(this, "提示", "连接未就绪"); return; }
    m_uartWidget->readAll();

    OTAController::CheckMode mode = (ui->ProtocolBox->currentText().trimmed() == "CRC16-2-2")
        ? OTAController::CHECK_MODBUS : OTAController::CHECK_NONE;
    m_otaController->startUpdate(m_currentFilePath, mode);
}

void MainWindow::sendData(const QByteArray &data)
{
    if (ui->tabWidget->currentIndex() == 0) m_uartWidget->sendData(data);
    else m_tcpWidget->sendData(data);
}

void MainWindow::onOtaProgress(const QString &msg, int type)
{
    static const QString colors[] = {"#0066CC", "blue", "#FF0000"};
    appendLog(msg, SystemMsg, colors[type < 0 || type > 2 ? 0 : type]);
}

void MainWindow::onOtaFinished(bool success, const QString &msg)
{
    appendLog(success ? ("✓ " + msg) : ("✗ " + msg), SystemMsg, success ? "#009900" : "#FF0000");
}

void MainWindow::sendText()
{
    QString input = ui->textEdit->toPlainText().trimmed();
    if (input.isEmpty() || !isConnected()) return;

    QByteArray rawData;
    if (m_isHexMode) {
        rawData = QByteArray::fromHex(
            input.remove(' ').remove(',').remove("0x", Qt::CaseInsensitive).remove("0X").toUtf8());
        if (rawData.isEmpty()) { QMessageBox::warning(this, "提示", "HEX格式无效"); return; }
        quint16 sum = 0;
        for (char b : rawData) sum += static_cast<quint8>(b);
        rawData.append(static_cast<char>(sum));
    } else {
        rawData = input.toUtf8();
    }

    sendData(rawData);
    appendLog(input, Sent);
}

void MainWindow::showReceivedData(const QByteArray &data)
{
    if (data.isEmpty()) return;
    QString display = m_isHexMode ? formatHex(data) : QString::fromUtf8(data);
    appendLog(display, Received);
}

void MainWindow::showSystemMessage(const QString &msg)
{
    appendLog(msg, SystemMsg, "#0066CC");
}

void MainWindow::appendLog(const QString &text, LogType type, const QString &color)
{
    static const char *aligns[] = {"right", "left", "center"};
    int i = static_cast<int>(type);
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString safe = text.toHtmlEscaped().replace("\n", "<br/>");

    QString html;
    if (type == SystemMsg) {
        html = QString("<table width='100%'><tr><td align='center'>"
                       "<font size='2' color='%1'>%2</font></td></tr></table>").arg(color, safe);
    } else {
        html = QString("<table width='100%'><tr><td align='%1'>"
                       "<font size='2' color='#666666'>%2 %3</font></td></tr>"
                       "<tr><td align='%1'><font face='Consolas, Monaco, monospace'>%4</font></td></tr></table>")
                       .arg(aligns[i], ts, m_isHexMode ? "hex" : "ascii", safe);
    }
    ui->textBrowser->append(html);
    ui->textBrowser->moveCursor(QTextCursor::End);
}

bool MainWindow::isConnected() const
{
    return ui->tabWidget->currentIndex() == 0
        ? m_uartWidget->isSerialConnected()
        : m_tcpWidget->isConnected();
}

QString MainWindow::formatHex(const QByteArray &data)
{
    QStringList list;
    for (char b : data)
        list << QString("0x%1").arg(static_cast<quint8>(b), 2, 16, QChar('0')).toUpper();
    return list.join(", ");
}
