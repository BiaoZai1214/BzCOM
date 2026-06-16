#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QTimer>
#include <QShortcut>
#include "comwidget.h"
#include "otacontroller.h"

class LogPanel;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void initUi(void);
    void initSignalSlots(void);
    OTAController::CheckMode getCheckMode(void) const;
    ComWidget* currentWidget(void) const;
    bool isConnected(void) const;
    void sendData(const QByteArray &data);

    void toggleHexMode(void);
    void updateChecksum(void);
    void selectFile(void);
    void sendText(void);
    void startFileOta(void);
    void onOtaProgress(const QString &msg, int type);
    void onOtaFinished(bool success, const QString &msg);
    void setOtaBusy(bool busy);
    QByteArray parseHexInput(const QString &input);
    QByteArray calculateChecksum(const QByteArray &data, int type);

    // ---- 新增功能 ----
    void setupExtraUi(void);
    void setupSendHistory(void);
    void setupHexFormatShortcuts(void);
    void setupGlobalShortcuts(void);

    void formatHexInput(void);
    void addToSendHistory(const QString &text);
    bool eventFilter(QObject *obj, QEvent *event) override;

    Ui::MainWindow *ui;
    ComWidget *m_widgets[3];
    OTAController *m_otaController;
    LogPanel *m_log;
    bool m_isHexMode = false;
    bool m_otaBusy = false;
    QString m_currentFilePath;

    // ---- 发送历史回显 ----
    QStringList m_sendHistory;             // 最多100条
    int m_historyIndex = -1;               // -1=未浏览
    QString m_savedInputBeforeHistory;

    // ---- 快捷键 ----
    QShortcut *m_formatHexShortcut = nullptr;
    QShortcut *m_textEditSendShortcut = nullptr;
};

#endif
