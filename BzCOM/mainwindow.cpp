#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uartwidget.h"
#include "tcpwidget.h"
#include "otaprotocol.h"
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QEventLoop>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_uartWidget(nullptr)
    , m_tcpWidget(nullptr)
    , m_isHexMode(false)
    , m_currentFilePath()
{
    ui->setupUi(this);
    initUI();
    initWidgets();
    initConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 初始化
void MainWindow::initUI()
{
    setWindowTitle("标仔IAP助手");
    ui->checkBox->setEditable(false);
    ui->checkBox->setEditText("校验和 (0x00)");
    ui->hexBtn->setText("ASCII");
    ui->textBrowser->setWordWrapMode(QTextOption::WrapAnywhere);
}

void MainWindow::initWidgets()
{
    m_uartWidget = new uartWidget(this);
    m_tcpWidget  = new tcpWidget(this);
    ui->tabWidget->insertTab(0, m_uartWidget, tr("串口"));
    ui->tabWidget->insertTab(1, m_tcpWidget,  tr("TCP"));
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::initConnections()
{
    connect(ui->clearLog,  &QPushButton::clicked,   this, &MainWindow::clearLog);
    connect(ui->saveLog,   &QPushButton::clicked,   this, &MainWindow::saveLog);
    connect(ui->sendBtn,   &QPushButton::clicked,   this, &MainWindow::sendText);
    connect(ui->hexBtn,    &QPushButton::clicked,   this, &MainWindow::toggleHexMode);
    connect(ui->selectFile,&QPushButton::clicked,   this, &MainWindow::selectFile);
    connect(ui->fileBtn,   &QPushButton::clicked,   this, &MainWindow::sendFile);
    connect(ui->textEdit,  &QTextEdit::textChanged, this, &MainWindow::updateChecksum);

    connect(m_uartWidget, &uartWidget::dataReceived, this, &MainWindow::showReceivedData);
    connect(m_tcpWidget,  &tcpWidget::dataReceived,  this, &MainWindow::showReceivedData);
    connect(m_uartWidget, &uartWidget::systemMessage,this, &MainWindow::showSystemMessage);
    connect(m_tcpWidget,  &tcpWidget::systemMessage, this, &MainWindow::showSystemMessage);
}

// UI 操作
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
        QByteArray data = QByteArray::fromHex(cleanHex(ui->textEdit->toPlainText()).toUtf8());
        if (!data.isEmpty())
            text = QString("校验和 (0x%1)").arg(calcChecksum(data), 2, 16, QChar('0')).toUpper();
    }

    ui->checkBox->setItemText(1, text);
}

void MainWindow::selectFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("选择 IAP 升级文件"), QDir::homePath(),
        tr("BIN 文件 (*.bin);;HEX 文件 (*.hex);;所有文件 (*.*)"));
    if (path.isEmpty()) return;

    m_currentFilePath = path;

    // 更新 selectFileBtn 文本为文件名
    ui->selectFile->setText(QFileInfo(path).fileName());

    appendLog(QString("已选择文件：%1（大小：%2 字节）")
                  .arg(QFileInfo(path).fileName())
                  .arg(QFileInfo(path).size()), SystemMsg, "#0066CC");
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
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件：") + file.errorString());
        return;
    }
    QTextStream(&file) << log;
    file.close();
    QMessageBox::information(this, tr("成功"), tr("日志已保存至：\n") + fileName);
}

// 获取当前校验模式
MainWindow::CheckMode MainWindow::getCurrentCheckMode()
{
    QString mode = ui->ProtocolBox->currentText().trimmed();
    if (mode == "CRC16-2-2") {
        return CHECK_MODBUS;
    }
    return CHECK_NONE;
}

// 数据发送 - 发送文本
void MainWindow::sendText()
{
    QString input = ui->textEdit->toPlainText().trimmed();
    if (input.isEmpty()) {
        QMessageBox::information(this, "提示", "输入内容为空，无法发送！");
        return;
    }
    if (!isConnectionReady()) {
        QMessageBox::warning(this, "提示", "连接未就绪，无法发送数据！");
        return;
    }

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
        display  = input;
        rawData  = input.toUtf8();
    }

    sendDataToActive(rawData);
    appendLog(display, Sent);
}

// 发送文件
void MainWindow::sendFile()
{
    if (m_currentFilePath.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择文件！");
        return;
    }

    if (!isConnectionReady()) {
        QMessageBox::warning(this, "提示", "连接未就绪，无法发送文件！");
        return;
    }

    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件：" + file.errorString());
        return;
    }

    qint64 totalSize = file.size();
    if (totalSize == 0) {
        QMessageBox::information(this, "提示", "文件为空！");
        file.close();
        return;
    }

    CheckMode mode = getCurrentCheckMode();

    if (mode == CHECK_NONE) {
        // 无校验模式：直接发送裸数据流
        appendLog(QString("--- 开始发送文件 (无校验) ---"), SystemMsg, "#0066CC");
        appendLog(QString("文件: %1, 大小: %2 字节")
                      .arg(QFileInfo(m_currentFilePath).fileName())
                      .arg(totalSize), SystemMsg, "#0066CC");

        int pktNum = 0;
        while (!file.atEnd()) {
            QByteArray buf = file.read(PACKET_SIZE);
            if (buf.isEmpty()) break;
            ++pktNum;
            sendDataToActive(buf);
            appendLog(QString("➤ 发送第 %1 包 (%2 字节)")
                          .arg(pktNum).arg(buf.size()), SystemMsg, "blue");
        }

        file.close();
        appendLog(QString("文件发送完成，共 %1 包").arg(pktNum), SystemMsg, "#009900");
        return;
    }

    // Modbus 模式：使用协议帧
    appendLog(QString("--- 开始发送文件 (CRC16-2-2 直接协议) ---"), SystemMsg, "#0066CC");
    appendLog(QString("文件: %1, 大小: %2 字节")
                  .arg(QFileInfo(m_currentFilePath).fileName())
                  .arg(totalSize), SystemMsg, "#0066CC");

    // 清空串口残留数据
    m_uartWidget->readAll();

    // ---------- 发送 CMD_UPDATE_START 帧 ----------
    QByteArray startPayload;
    startPayload.reserve(4);
    startPayload.append(static_cast<char>((totalSize >> 0) & 0xFF));
    startPayload.append(static_cast<char>((totalSize >> 8) & 0xFF));
    startPayload.append(static_cast<char>((totalSize >> 16) & 0xFF));
    startPayload.append(static_cast<char>((totalSize >> 24) & 0xFF));
    QByteArray startFrame = OtaProtocol::instance().buildFrame(CMD_UPDATE_START, startPayload);

    appendLog(QString("[DEBUG] 帧长度=%1, 十六进制: %2")
                  .arg(startFrame.size())
                  .arg(startFrame.toHex(' ').toUpper().constData()), SystemMsg, "#FF00FF");

    appendLog(QString("[DEBUG] 等待下位机准备..."), SystemMsg, "#FF0000");
    QThread::msleep(100);  // 等待下位机准备接收

    appendLog(QString("[DEBUG] 开始发送..."), SystemMsg, "#FF0000");
    sendDataToActive(startFrame);
    appendLog(QString("[DEBUG] 发送完成"), SystemMsg, "#FF0000");
    appendLog(QString("➤ 发送升级开始帧, 总大小=%1").arg(totalSize), SystemMsg, "blue");

    // 等待下位机响应 (解析协议帧 ACK)
    {
        QByteArray recvBuf;
        bool gotAck = false;
        QEventLoop loop;
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        auto conn = connect(m_uartWidget, &uartWidget::dataReceived, [&](QByteArray data) {
            recvBuf.append(data);
            appendLog(QString("收到原始数据: %1 字节").arg(data.size()), SystemMsg, "#FF8800");
            appendLog(QString("累计缓冲: %1 字节").arg(recvBuf.size()), SystemMsg, "#FF8800");
            QByteArray respFrame = OtaProtocol::instance().parseFrame(recvBuf);
            if (!respFrame.isEmpty()) {
                appendLog(QString("帧解析成功: %1 字节").arg(respFrame.size()), SystemMsg, "#FF8800");
                if (OtaProtocol::instance().verifyCRC(respFrame)) {
                    quint16 respCmd = OtaProtocol::instance().getCmd(respFrame);
                    appendLog(QString("命令码: 0x%1 (期望 0x%2)")
                                  .arg(respCmd, 4, 16, QChar('0')).arg(CMD_UPDATE_START | 0x0080, 4, 16, QChar('0')), SystemMsg, "#FF8800");
                    if (respCmd == (CMD_UPDATE_START | 0x0080)) {
                        gotAck = true;
                        loop.quit();
                    }
                } else {
                    appendLog("CRC 校验失败", SystemMsg, "#FF0000");
                }
            } else {
                appendLog(QString("无法解析完整帧 (缓冲 %1 字节)").arg(recvBuf.size()), SystemMsg, "#FF8800");
            }
        });
        timeoutTimer.start(ACK_TIMEOUT_MS);
        connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
        loop.exec();
        disconnect(conn);

        if (!gotAck) {
            appendLog("❌ 等待 UPDATE_START 响应超时", SystemMsg, "#FF0000");
            if (!recvBuf.isEmpty()) {
                appendLog(QString("接收缓冲残留: %1 字节").arg(recvBuf.size()), SystemMsg, "#FF0000");
            }
            file.close();
            return;
        }
        appendLog("✓ 收到 UPDATE_START ACK", SystemMsg, "#009900");
    }

    // ---------- 发送数据帧 ----------
    int pktNum = 0;
    qint64 sentSize = 0;

    while (!file.atEnd()) {
        QByteArray buf = file.read(PACKET_SIZE);
        if (buf.isEmpty()) break;
        ++pktNum;
        qint64 offset = sentSize;
        sentSize += buf.size();

        // 构建数据区: 偏移(4B) + 数据
        QByteArray payload;
        payload.reserve(4 + buf.size());
        payload.append(static_cast<char>((offset >> 0) & 0xFF));
        payload.append(static_cast<char>((offset >> 8) & 0xFF));
        payload.append(static_cast<char>((offset >> 16) & 0xFF));
        payload.append(static_cast<char>((offset >> 24) & 0xFF));
        payload.append(buf);

        // 构建协议帧
        QByteArray frame = OtaProtocol::instance().buildFrame(CMD_UPDATE_DATA, payload);
        sendDataToActive(frame);

        appendLog(QString("➤ 发送第 %1 包, 偏移=%2, 大小=%3")
                      .arg(pktNum).arg(offset).arg(buf.size()), SystemMsg, "blue");

        // 等待下位机响应 (解析协议帧 ACK)
        {
            QByteArray recvBuf;
            bool gotAck = false;
            QEventLoop loop;
            QTimer timeoutTimer;
            timeoutTimer.setSingleShot(true);
            auto conn = connect(m_uartWidget, &uartWidget::dataReceived, [&](QByteArray data) {
                recvBuf.append(data);
                QByteArray respFrame = OtaProtocol::instance().parseFrame(recvBuf);
                if (!respFrame.isEmpty() && OtaProtocol::instance().verifyCRC(respFrame)) {
                    quint16 respCmd = OtaProtocol::instance().getCmd(respFrame);
                    gotAck = (respCmd == (CMD_UPDATE_DATA | 0x0080));
                    loop.quit();
                }
            });
            timeoutTimer.start(ACK_TIMEOUT_MS);
            connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
            loop.exec();
            disconnect(conn);

            if (!gotAck) {
                appendLog(QString("❌ 第 %1 包响应超时或 NAK").arg(pktNum), SystemMsg, "#FF0000");
                file.close();
                return;
            }
        }
    }

    file.close();

    // ---------- 发送结束帧 ----------
    QByteArray endPayload;
    endPayload.reserve(4);
    endPayload.append(static_cast<char>((totalSize >> 0) & 0xFF));
    endPayload.append(static_cast<char>((totalSize >> 8) & 0xFF));
    endPayload.append(static_cast<char>((totalSize >> 16) & 0xFF));
    endPayload.append(static_cast<char>((totalSize >> 24) & 0xFF));
    QByteArray endFrame = OtaProtocol::instance().buildFrame(CMD_UPDATE_END, endPayload);
    sendDataToActive(endFrame);
    appendLog("➤ 发送升级结束帧", SystemMsg, "blue");

    // 等待结束帧 ACK（下位机需要烧录 Flash，给更长超时）
    {
        QByteArray recvBuf;
        bool gotAck = false;
        QEventLoop loop;
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        auto conn = connect(m_uartWidget, &uartWidget::dataReceived, [&](QByteArray data) {
            recvBuf.append(data);
            QByteArray respFrame = OtaProtocol::instance().parseFrame(recvBuf);
            if (!respFrame.isEmpty() && OtaProtocol::instance().verifyCRC(respFrame)) {
                quint16 respCmd = OtaProtocol::instance().getCmd(respFrame);
                gotAck = (respCmd == (CMD_UPDATE_END | 0x0080));
                loop.quit();
            }
        });
        timeoutTimer.start(ACK_TIMEOUT_MS * 3);  // 烧录需要更长时间
        connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
        loop.exec();
        disconnect(conn);

        if (gotAck) {
            appendLog(QString("✓ OTA 升级成功，共 %1 包").arg(pktNum), SystemMsg, "#009900");
        } else {
            appendLog("❌ OTA 升级失败或超时", SystemMsg, "#FF0000");
        }
    }
}

// 日志显示
void MainWindow::showReceivedData(QByteArray data)
{
    if (data.isEmpty()) return;

    // 过滤掉可显示字符（只保留需要显示的数据）
    QString display = formatData(data, m_isHexMode);
    if (display.isEmpty()) return;

    appendLog(display, Received);
}

void MainWindow::showSystemMessage(const QString &msg)
{
    appendLog(msg, SystemMsg, "#0066CC");
}

void MainWindow::appendLog(const QString &text, LogType type, const QString &color)
{
    static const char* aligns[] = {"right", "left", "center"};

    int i = static_cast<int>(type);
    QString ts   = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString mode = m_isHexMode ? "hex" : "ascii";
    QString safe = text.toHtmlEscaped().replace("\r\n", "<br/>").replace("\n", "<br/>");

    QString html;
    if (type == SystemMsg) {
        html = QString("<table width='100%'><tr><td align='center'>"
                       "<font size='2' color='%1'>%2</font>"
                       "</td></tr></table>").arg(color, safe);
    } else {
        html = QString("<table width='100%'><tr><td align='%1'>"
                       "<font size='2' color='#666666'>%2 %3</font>"
                       "</td></tr><tr><td align='%1'>"
                       "<font face='Consolas, Monaco, monospace'>%4</font>"
                       "</td></tr></table>").arg(aligns[i], ts, mode, safe);
    }

    ui->textBrowser->append(html);
    ui->textBrowser->moveCursor(QTextCursor::End);
}

// 辅助函数
quint8 MainWindow::calcChecksum(const QByteArray &data)
{
    quint16 sum = 0;
    for (char b : data) sum += static_cast<quint8>(b);
    return static_cast<quint8>(sum & 0xFF);
}

QString MainWindow::cleanHex(const QString &text)
{
    return QString(text).remove(" ").remove(",")
    .remove("0x", Qt::CaseInsensitive).remove("0X", Qt::CaseInsensitive);
}

QString MainWindow::formatData(const QByteArray &data, bool hex)
{
    if (hex) {
        QStringList list;
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
