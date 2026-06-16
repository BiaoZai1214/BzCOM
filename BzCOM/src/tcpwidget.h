#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include "comwidget.h"
#include <QStackedWidget>

namespace Ui { class tcpWidget; }

class tcpWidget : public ComWidget
{
    Q_OBJECT

public:
    explicit tcpWidget(QWidget *parent = nullptr);
    ~tcpWidget() override;

    bool isConnected(void) const override;
    void sendData(const QByteArray &data) override;

private slots:
    void onModeChanged(int idx);

private:
    void routeSignal(ComWidget *w);

    Ui::tcpWidget *ui;
    ComWidget *m_widgets[3];
};

#endif
