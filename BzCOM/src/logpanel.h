#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

class QTextBrowser;

class LogPanel : public QObject
{
    Q_OBJECT

public:
    explicit LogPanel(QTextBrowser *browser, QObject *parent = nullptr);

    enum Type { Sent = 0, Received = 1, System = 2 };

    void setHexMode(bool on) { m_hexMode = on; }
    bool hexMode() const { return m_hexMode; }

    void appendData(const QByteArray &data);
    void appendMessage(const QString &msg, const QString &color = "#0066CC");
    void appendSent(const QString &text);
    void clear();
    void save();

private:
    void appendRaw(const QString &text, Type type, const QString &color = QString());
    void appendAscii(const QString &text);
    void flushAscii(bool force);
    void trim();
    QString formatHex(const QByteArray &data);

    QTextBrowser *m_browser;
    bool m_hexMode = false;
    QString m_asciiBuf;
    QTimer m_flushTimer;
    static constexpr int MAX_BLOCKS = 2000;
};

#endif
