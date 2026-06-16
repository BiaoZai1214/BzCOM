#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uartwidget.h"
#include "tcpwidget.h"
#include "canwidget.h"
#include "logpanel.h"
#include "otacontroller.h"
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_otaController(new OTAController(this))
    , m_log(nullptr)
{
    for (int i = 0; i < 3; i++) m_widgets[i] = nullptr;
    ui->setupUi(this);
    m_log = new LogPanel(ui->textBrowser, this);
    initUi();
    initSignalSlots();
    setupExtraUi();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initUi(void)
{
    setWindowTitle("BzCOM");
    m_widgets[0] = new uartWidget(this);
    m_widgets[1] = new tcpWidget(this);
    m_widgets[2] = new canWidget(this);
    ui->tabWidget->insertTab(0, m_widgets[0], tr("串口"));
    ui->tabWidget->insertTab(1, m_widgets[1], tr("TCP"));
    ui->tabWidget->insertTab(2, m_widgets[2], tr("CAN"));
}


ComWidget* MainWindow::currentWidget(void) const {
    int idx = ui->tabWidget->currentIndex();
    if (idx < 0 || idx >= 3 || !m_widgets[idx]) return nullptr;
    return m_widgets[idx];
}

bool MainWindow::isConnected(void) const {
    auto *w = currentWidget();
    return w && w->isConnected();
}

void MainWindow::sendData(const QByteArray &data) {
    auto *w = currentWidget();
    if (w) w->sendData(data);
}
void MainWindow::initSignalSlots(void)
{
    connect(ui->clearLog, &QPushButton::clicked, m_log, &LogPanel::clear);
    connect(ui->saveLog, &QPushButton::clicked, m_log, &LogPanel::save);
    connect(ui->sendBtn, &QPushButton::clicked, this, &MainWindow::sendText);
    connect(ui->hexBtn, &QPushButton::clicked, this, &MainWindow::toggleHexMode);
    connect(ui->selectFile, &QPushButton::clicked, this, &MainWindow::selectFile);
    connect(ui->fileBtn, &QPushButton::clicked, this, &MainWindow::startFileOta);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::updateChecksum);
    connect(ui->checkBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateChecksum);

    for (int i = 0; i < 3; i++) {
        connect(m_widgets[i], &ComWidget::dataReceived, m_log, &LogPanel::appendData);
        connect(m_widgets[i], &ComWidget::systemMessage, this, [this](const QString &m) { m_log->appendMessage(m); });
    }

    connect(m_widgets[0], &ComWidget::dataReceived, m_otaController, &OTAController::onDataReceived);
    connect(m_widgets[1], &ComWidget::dataReceived, m_otaController, &OTAController::onDataReceived);
    connect(m_otaController, &OTAController::sendData, this, &MainWindow::sendData);
    connect(m_otaController, &OTAController::progress, this, &MainWindow::onOtaProgress);
    connect(m_otaController, &OTAController::otaFinished, this, &MainWindow::onOtaFinished);
}

OTAController::CheckMode MainWindow::getCheckMode(void) const
{
    return (ui->ProtocolBox->currentText().trimmed() == "CRC16-2-2")
        ? OTAController::CHECK_MODBUS : OTAController::CHECK_NONE;
}

void MainWindow::toggleHexMode(void)
{
    m_isHexMode = !m_isHexMode;
    m_log->setHexMode(m_isHexMode);
    ui->hexBtn->setText(m_isHexMode ? "HEX" : "ASCII");
    updateChecksum();
}

void MainWindow::updateChecksum(void)
{
    QString noneText = "无校验";
    QString sum8Text = "校验和";
    QString xor8Text = "异或校验";
    QString crc16Text = "CRC16";

    QString input = ui->textEdit->toPlainText();
    if (!input.isEmpty()) {
        QByteArray data;
        if (m_isHexMode) {
            data = parseHexInput(input);
        } else {
            data = input.toUtf8();
        }

        if (!data.isEmpty()) {
            // 校验和 Sum8
            QByteArray cs = calculateChecksum(data, 1);
            if (!cs.isEmpty())
                sum8Text = QString("校验和 (0x%1)").arg(QString(cs.toHex()).toUpper());

            // 异或校验 XOR8
            cs = calculateChecksum(data, 2);
            if (!cs.isEmpty())
                xor8Text = QString("异或校验 (0x%1)").arg(QString(cs.toHex()).toUpper());

            // CRC16
            cs = calculateChecksum(data, 3);
            if (!cs.isEmpty())
                crc16Text = QString("CRC16 (0x%1)").arg(QString(cs.toHex()).toUpper());
        }
    }

    // 统一居中对齐：按最长文本补齐前导空格
    int maxLen = qMax(qMax(noneText.length(), sum8Text.length()),
                      qMax(xor8Text.length(), crc16Text.length()));
    auto padCenter = [maxLen](QString &s) {
        int left = (maxLen - s.length()) / 2;
        s = QString(left, ' ') + s;
    };
    padCenter(noneText);
    padCenter(sum8Text);
    padCenter(xor8Text);
    padCenter(crc16Text);

    ui->checkBox->setItemText(0, noneText);
    ui->checkBox->setItemText(1, sum8Text);
    ui->checkBox->setItemText(2, xor8Text);
    ui->checkBox->setItemText(3, crc16Text);
}

QByteArray MainWindow::parseHexInput(const QString &input)
{
    QString cleaned = input;
    cleaned.remove(' ').remove(',').remove("0x", Qt::CaseInsensitive).remove("0X");
    return QByteArray::fromHex(cleaned.toUtf8());
}

QByteArray MainWindow::calculateChecksum(const QByteArray &data, int type)
{
    if (data.isEmpty()) return QByteArray();

    switch (type) {
    case 1: { // 校验和 Sum8
        quint16 sum = 0;
        for (char b : data) sum += static_cast<quint8>(b);
        return QByteArray(1, static_cast<char>(sum & 0xFF));
    }
    case 2: { // 异或校验 XOR8
        quint8 x = 0;
        for (char b : data) x ^= static_cast<quint8>(b);
        return QByteArray(1, static_cast<char>(x));
    }
    case 3: { // CRC16 (Modbus)
        quint16 crc = 0xFFFF;
        for (char b : data) {
            crc ^= static_cast<quint8>(b);
            for (int j = 0; j < 8; j++) {
                if (crc & 0x01) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }
        QByteArray result;
        result.append(static_cast<char>(crc & 0xFF));   // 低字节在前
        result.append(static_cast<char>(crc >> 8));     // 高字节在后
        return result;
    }
    default: return QByteArray();
    }
}

void MainWindow::selectFile(void)
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择文件"), QDir::homePath(),
        tr("BIN文件(*.bin);;HEX文件(*.hex);;所有文件(*.*)"));
    if (path.isEmpty()) return;
    m_currentFilePath = path;
    QFileInfo fi(path);
    ui->selectFile->setText(fi.fileName());
    m_log->appendMessage(QString("已选择：%1（%2 字节）").arg(fi.fileName()).arg(fi.size()));
}

void MainWindow::startFileOta(void)
{
    if (m_currentFilePath.isEmpty()) { QMessageBox::information(this, "提示", "请先选择文件"); return; }
    if (!isConnected()) { QMessageBox::warning(this, "提示", "连接未就绪"); return; }
    if (m_otaBusy) { QMessageBox::information(this, "提示", "OTA正在进行中"); return; }

    // 清空串口接收缓冲区（仅UART模式）
    if (ui->tabWidget->currentIndex() == 0 && m_widgets[0]) {
        auto *uart = dynamic_cast<uartWidget*>(m_widgets[0]);
        if (uart) uart->readAll();
    }

    setOtaBusy(true);
    m_otaController->startUpdate(m_currentFilePath, getCheckMode());
}

void MainWindow::onOtaProgress(const QString &msg, int type)
{
    static const QString colors[] = {"#0066CC", "blue", "#FF0000"};
    m_log->appendMessage(msg, colors[type < 0 || type > 2 ? 0 : type]);
}

void MainWindow::onOtaFinished(bool success, const QString &msg)
{
    setOtaBusy(false);
    m_log->appendMessage(success ? ("✓ " + msg) : ("✗ " + msg),
                         success ? "#009900" : "#FF0000");
}

void MainWindow::sendText(void)
{
    if (m_otaBusy) { QMessageBox::information(this, "提示", "OTA过程中禁止手动发送"); return; }
    QString input = ui->textEdit->toPlainText().trimmed();
    if (input.isEmpty() || !isConnected()) return;
    QByteArray rawData;
    if (m_isHexMode) {
        rawData = parseHexInput(input);
        if (rawData.isEmpty()) { QMessageBox::warning(this, "提示", "HEX格式无效"); return; }
    } else {
        rawData = input.toUtf8();
    }

    // 追加校验字节
    int csType = ui->checkBox->currentIndex();
    if (csType > 0) {
        QByteArray cs = calculateChecksum(rawData, csType);
        if (!cs.isEmpty()) rawData.append(cs);
    }

    sendData(rawData);
    m_log->appendSent(input);

    // 记录发送历史
    addToSendHistory(input);
    m_historyIndex = -1;
}

void MainWindow::setOtaBusy(bool busy)
{
    m_otaBusy = busy;
    ui->fileBtn->setEnabled(!busy);
    ui->selectFile->setEnabled(!busy);
    ui->sendBtn->setEnabled(!busy);
    ui->tabWidget->setEnabled(!busy);
}

// ===================== 新增功能 ==========================================

void MainWindow::setupExtraUi(void)
{
    setupSendHistory();
    setupHexFormatShortcuts();
    setupGlobalShortcuts();
}

// ---- 发送历史回显 -------------------------------------------------------

void MainWindow::setupSendHistory(void)
{
    ui->textEdit->installEventFilter(this);
}

void MainWindow::addToSendHistory(const QString &text)
{
    if (text.isEmpty()) return;
    if (!m_sendHistory.isEmpty() && m_sendHistory.last() == text) return;
    m_sendHistory.append(text);
    if (m_sendHistory.size() > 100) m_sendHistory.removeFirst();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->textEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        QTextCursor cursor = ui->textEdit->textCursor();

        // ↑ 历史回退 -- 光标在文本最开头
        if (ke->key() == Qt::Key_Up && ke->modifiers() == Qt::NoModifier) {
            if (cursor.position() == 0 && !cursor.hasSelection()) {
                if (!m_sendHistory.isEmpty()) {
                    if (m_historyIndex == -1) {
                        m_savedInputBeforeHistory = ui->textEdit->toPlainText();
                        m_historyIndex = m_sendHistory.size() - 1;
                    } else {
                        m_historyIndex = qMax(0, m_historyIndex - 1);
                    }
                    ui->textEdit->setPlainText(m_sendHistory.at(m_historyIndex));
                    ui->textEdit->moveCursor(QTextCursor::End);
                }
                return true;
            }
        }

        // ↓ 历史前进 -- 光标在文本最末尾
        if (ke->key() == Qt::Key_Down && ke->modifiers() == Qt::NoModifier) {
            if (cursor.atEnd() && !cursor.hasSelection()) {
                if (m_historyIndex >= 0) {
                    m_historyIndex++;
                    if (m_historyIndex >= m_sendHistory.size()) {
                        m_historyIndex = -1;
                        ui->textEdit->setPlainText(m_savedInputBeforeHistory);
                        m_savedInputBeforeHistory.clear();
                    } else {
                        ui->textEdit->setPlainText(m_sendHistory.at(m_historyIndex));
                    }
                    ui->textEdit->moveCursor(QTextCursor::End);
                    return true;
                }
            }
        }

        // Esc 退出历史浏览
        if (ke->key() == Qt::Key_Escape && m_historyIndex >= 0) {
            m_historyIndex = -1;
            ui->textEdit->setPlainText(m_savedInputBeforeHistory);
            m_savedInputBeforeHistory.clear();
            ui->textEdit->moveCursor(QTextCursor::End);
            return true;
        }

        // 任意其他按键 → 退出历史浏览
        if (m_historyIndex >= 0 && !ke->text().isEmpty()
            && ke->key() != Qt::Key_Shift && ke->key() != Qt::Key_Control
            && ke->key() != Qt::Key_Alt && ke->key() != Qt::Key_Meta) {
            m_historyIndex = -1;
            m_savedInputBeforeHistory.clear();
        }
    }

    // 失去焦点时退出历史浏览
    if (obj == ui->textEdit && event->type() == QEvent::FocusOut) {
        m_historyIndex = -1;
    }

    return QMainWindow::eventFilter(obj, event);
}

// ---- HEX格式整理 --------------------------------------------------------

void MainWindow::setupHexFormatShortcuts(void)
{
    // 右键菜单
    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit, &QTextEdit::customContextMenuRequested,
            this, [this](const QPoint &pos) {
        QMenu *menu = ui->textEdit->createStandardContextMenu();
        menu->addSeparator();
        QAction *fmtAction = menu->addAction("格式化 HEX\tCtrl+T");
        connect(fmtAction, &QAction::triggered, this, &MainWindow::formatHexInput);
        menu->exec(ui->textEdit->mapToGlobal(pos));
        delete menu;
    });

    // Ctrl+T 快捷键 (MainWindow级，任意子控件聚焦均生效)
    m_formatHexShortcut = new QShortcut(QKeySequence("Ctrl+T"), this);
    m_formatHexShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_formatHexShortcut, &QShortcut::activated,
            this, &MainWindow::formatHexInput);
}

void MainWindow::formatHexInput(void)
{
    if (!m_isHexMode) return;

    QString input = ui->textEdit->toPlainText();
    if (input.isEmpty()) return;

    // 只保留十六进制字符
    static const QRegularExpression nonHexRe("[^0-9A-Fa-f]");
    QString cleaned = input;
    cleaned.remove(nonHexRe);
    if (cleaned.isEmpty()) {
        ui->textEdit->clear();
        return;
    }

    // 奇数位补0
    if (cleaned.length() % 2 != 0)
        cleaned.prepend('0');

    cleaned = cleaned.toUpper();

    // 两字符一组加空格
    QStringList groups;
    for (int i = 0; i < cleaned.length(); i += 2)
        groups << cleaned.mid(i, 2);

    ui->textEdit->setPlainText(groups.join(' '));
    ui->textEdit->moveCursor(QTextCursor::End);
}

// ---- 全局快捷键 ---------------------------------------------------------

void MainWindow::setupGlobalShortcuts(void)
{
    // Shift+Enter 发送 (仅textEdit聚焦时生效)
    m_textEditSendShortcut = new QShortcut(QKeySequence("Shift+Return"), ui->textEdit);
    connect(m_textEditSendShortcut, &QShortcut::activated,
            this, &MainWindow::sendText);
}
