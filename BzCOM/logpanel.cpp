#include "logpanel.h"
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocument>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

LogPanel::LogPanel(QTextBrowser *browser, QObject *parent)
    : QObject(parent)
    , m_browser(browser)
{
    m_flushTimer.setSingleShot(true);
    m_flushTimer.setInterval(80);
    connect(&m_flushTimer, &QTimer::timeout, this, [this]() { flushAscii(true); });
}

// ---- public ----------------------------------------------------------

void LogPanel::appendData(const QByteArray &data)
{
    if (data.isEmpty()) return;
    if (m_hexMode) {
        flushAscii(true);
        appendRaw(formatHex(data), Received);
    } else {
        appendAscii(QString::fromUtf8(data));
    }
}

void LogPanel::appendMessage(const QString &msg, const QString &color)
{
    appendRaw(msg, System, color);
}

void LogPanel::appendSent(const QString &text)
{
    appendRaw(text, Sent);
}

void LogPanel::clear()
{
    m_asciiBuf.clear();
    m_flushTimer.stop();
    m_browser->clear();
}

void LogPanel::save()
{
    QString log = m_browser->toPlainText().trimmed();
    if (log.isEmpty()) { QMessageBox::information(qobject_cast<QWidget*>(parent()), "提示", "日志为空"); return; }
    QString fileName = QFileDialog::getSaveFileName(qobject_cast<QWidget*>(parent()), "保存日志",
        QString("日志_%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "日志文件(*.log *.txt)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream(&file) << log; file.close();
        QMessageBox::information(qobject_cast<QWidget*>(parent()), "成功", "已保存");
    }
}

// ---- private ---------------------------------------------------------

void LogPanel::appendRaw(const QString &text, Type type, const QString &color)
{
    static const char *aligns[] = {"right", "left", "center"};
    int i = static_cast<int>(type);
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString safe = text.toHtmlEscaped().replace("\n", "<br/>");

    QString html;
    if (type == System) {
        html = QString("<table width='100%'><tr><td align='center'>"
                       "<font size='2' color='%1'>%2</font></td></tr></table>").arg(color, safe);
    } else {
        html = QString("<table width='100%'><tr><td align='%1'>"
                       "<font size='2' color='#666666'>%2 %3</font></td></tr>"
                       "<tr><td align='%1'><font face='Consolas, Monaco, monospace'>%4</font></td></tr></table>")
                       .arg(aligns[i], ts, m_hexMode ? "hex" : "ascii", safe);
    }
    m_browser->append(html);
    trim();
    m_browser->moveCursor(QTextCursor::End);
}

void LogPanel::appendAscii(const QString &text)
{
    if (text.isEmpty()) return;
    QString s = text;
    s.replace("\r\n", "\n").replace('\r', '\n');
    m_asciiBuf.append(s);
    flushAscii(false);
    if (!m_asciiBuf.isEmpty()) m_flushTimer.start();
}

void LogPanel::flushAscii(bool force)
{
    int pos;
    while ((pos = m_asciiBuf.indexOf('\n')) >= 0) {
        appendRaw(m_asciiBuf.left(pos), Received);
        m_asciiBuf.remove(0, pos + 1);
    }
    if (force && !m_asciiBuf.isEmpty()) {
        appendRaw(m_asciiBuf, Received);
        m_asciiBuf.clear();
    }
}

void LogPanel::trim()
{
    QTextDocument *doc = m_browser->document();
    if (!doc || doc->blockCount() <= MAX_BLOCKS) return;
    QTextCursor cursor(doc);
    cursor.movePosition(QTextCursor::Start);
    const int n = doc->blockCount() - MAX_BLOCKS;
    for (int i = 0; i < n; ++i) {
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
    }
}

QString LogPanel::formatHex(const QByteArray &data)
{
    QStringList list;
    for (char b : data)
        list << QString("0x%1").arg(static_cast<quint8>(b), 2, 16, QChar('0')).toUpper();
    return list.join(", ");
}
