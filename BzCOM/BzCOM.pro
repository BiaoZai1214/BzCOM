QT += widgets serialport network

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tcpwidget.cpp \
    uartwidget.cpp \
    otaprotocol.cpp

HEADERS += \
    mainwindow.h \
    tcpwidget.h \
    uartwidget.h \
    otaprotocol.h

FORMS += \
    mainwindow.ui \
    tcpwidget.ui \
    uartwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
