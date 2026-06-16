QT += widgets serialport network

CONFIG += c++11

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/logpanel.cpp \
    src/mainwindow.cpp \
    src/tcpwidget.cpp \
    src/tcpclientwidget.cpp \
    src/tcpserverwidget.cpp \
    src/udpwidget.cpp \
    src/uartwidget.cpp \
    src/canwidget.cpp \
    src/otaprotocol.cpp \
    src/otacontroller.cpp \
    src/checksumutil.cpp

HEADERS += \
    src/comwidget.h \
    src/logpanel.h \
    src/mainwindow.h \
    src/tcpwidget.h \
    src/tcpclientwidget.h \
    src/tcpserverwidget.h \
    src/udpwidget.h \
    src/uartwidget.h \
    src/canwidget.h \
    src/otaprotocol.h \
    src/otacontroller.h \
    src/checksumutil.h

FORMS += \
    forms/mainwindow.ui \
    forms/uartwidget.ui \
    forms/canwidget.ui \
    forms/tcpwidget.ui \
    forms/tcpclientwidget.ui \
    forms/tcpserverwidget.ui \
    forms/udpwidget.ui

RESOURCES += \
    res/image.qrc

RC_ICONS = res/bz.ico
