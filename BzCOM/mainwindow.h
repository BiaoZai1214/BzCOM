#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class OTAController;
class uartWidget;
class tcpWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum LogType { Sent = 0, Received = 1, SystemMsg = 2 };

private:
    void toggleHexMode();
    void updateChecksum();
    void selectFile();
    void clearLog();
    void saveLog();
    void sendText();
    void onFileClicked();
    void sendData(const QByteArray &data);
    void onOtaProgress(const QString &msg, int type);
    void onOtaFinished(bool success, const QString &msg);
    void showReceivedData(const QByteArray &data);
    void showSystemMessage(const QString &msg);
    void appendLog(const QString &text, LogType type, const QString &color = QString());
    bool isConnected() const;
    QString formatHex(const QByteArray &data);

    Ui::MainWindow *ui;
    OTAController *m_otaController;
    uartWidget *m_uartWidget;
    tcpWidget *m_tcpWidget;
    bool m_isHexMode;
    QString m_currentFilePath;
};

#endif
