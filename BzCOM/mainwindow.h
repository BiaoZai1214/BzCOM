#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QTimer>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class uartWidget;
class tcpWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // ---------- 日志类型枚举 ----------
    enum LogType { Sent = 0, Received = 1, SystemMsg = 2 };

    // ---------- 通信常量 ----------
    static constexpr int    PACKET_SIZE     = 240;   // 固件数据每包大小
    static constexpr int    ACK_TIMEOUT_MS  = 3000;  // OTA 响应超时

    // ---------- 校验模式 ----------
    enum CheckMode {
        CHECK_NONE = 0,      // 无校验
        CHECK_MODBUS = 1     // Modbus 风格协议帧
    };

private:
    // ---------- 初始化 ----------
    void initUI();
    void initWidgets();
    void initConnections();

    // ---------- UI 操作 ----------
    void toggleHexMode();
    void updateChecksum();
    void selectFile();
    void clearLog();
    void saveLog();

    // ---------- 数据发送 ----------
    void sendText();
    void sendFile();

    // ---------- 日志显示 ----------
    void showReceivedData(QByteArray data);
    void showSystemMessage(const QString &msg);
    void appendLog(const QString &text,
                   LogType type,
                   const QString &color = QString());

    // ---------- 辅助函数 ----------
    quint8   calcChecksum(const QByteArray &data);
    QString  cleanHex(const QString &text);
    QString  formatData(const QByteArray &data, bool hex);

    void sendDataToActive(const QByteArray &data);
    bool isConnectionReady() const;
    CheckMode getCurrentCheckMode();

    // ---------- 成员变量 ----------
    Ui::MainWindow *ui;
    uartWidget *m_uartWidget;
    tcpWidget  *m_tcpWidget;

    bool    m_isHexMode;
    QString m_currentFilePath;   // 当前选择的文件路径
};

#endif // MAINWINDOW_H
