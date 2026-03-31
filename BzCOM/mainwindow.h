#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class OTAController;
class uartWidget;
class tcpWidget;

/**
 * @brief 主窗口（UI 层）
 * @note 只负责界面布局和信号转发，业务逻辑全部委托给 OTAController
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum LogType { Sent = 0, Received = 1, SystemMsg = 2 };

private:
    // 初始化
    void initUi();
    void initConnections();

    // UI 操作
    void toggleHexMode();
    void updateChecksum();
    void selectFile();
    void clearLog();
    void saveLog();
    void sendText();
    void onFileBtnClicked();

    // OTA 回调
    void onOtaProgress(const QString &msg, int type);
    void onOtaFinished(bool success, const QString &msg);

    // 发送数据
    void sendDataToActive(const QByteArray &data);
    void onOtaSendData(const QByteArray &data);
    bool isConnectionReady() const;

    // 工具
    quint8  calcChecksum(const QByteArray &data);
    QString cleanHex(const QString &text);
    QString formatData(const QByteArray &data, bool hex);

    // 日志
    void showReceivedData(const QByteArray &data);
    void showSystemMessage(const QString &msg);
    void appendLog(const QString &text, LogType type, const QString &color = QString());

    Ui::MainWindow *ui;
    OTAController *m_otaController;
    uartWidget *m_uartWidget;
    tcpWidget  *m_tcpWidget;
    bool m_isHexMode;
    QString m_currentFilePath;
};

#endif
