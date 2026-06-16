#ifndef CANWIDGET_H
#define CANWIDGET_H

#include "comwidget.h"
#include <QLibrary>
#include <QTimer>

namespace Ui { class canWidget; }

class canWidget : public ComWidget
{
    Q_OBJECT

public:
    explicit canWidget(QWidget *parent = nullptr);
    ~canWidget() override;

    bool isConnected(void) const override;
    void sendData(const QByteArray &) override {}


private slots:
    void onConnectClicked(void);
    void onPollTimer(void);

private:
    bool loadPcanBasic(void);
    void doConnect(void);
    void doDisconnect(void);
    QString formatFrame(quint32 id, quint8 msgType, const quint8 *data, quint8 len);

    // PCANBasic
    QLibrary m_pcanLib;
    typedef quint32 (__stdcall *CAN_InitFunc)(quint16, quint16, quint8, quint32, quint16);
    typedef quint32 (__stdcall *CAN_ReadFunc)(quint16, void*, void*);
    typedef quint32 (__stdcall *CAN_SetValueFunc)(quint16, quint8, void*, quint32);
    typedef quint32 (__stdcall *CAN_UninitFunc)(quint16);
    CAN_InitFunc m_canInit;
    CAN_ReadFunc m_canRead;
    CAN_SetValueFunc m_canSetValue;
    CAN_UninitFunc m_canUninit;
    bool m_loaded;
    bool m_connected;
    quint16 m_channel;
    quint16 m_baudRate;

    Ui::canWidget *ui;
    QTimer *m_pollTimer;
};

#endif
