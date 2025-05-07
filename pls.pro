QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ModelARX.cpp \
    RegulatorPID.cpp \
    SiecUAR.cpp \
    dialog_arx.cpp \
    dialog_sieciowy.cpp \
    main.cpp \
    mainwindow.cpp \
    symulator.cpp
    RegulatorPID.cpp
    ModelARX.cpp

HEADERS += \
    ModelARX.h \
    RegulatorPID.h \
    SiecUAR.h \
    WartoscZadana.h \
    dialog_arx.h \
    dialog_sieciowy.h \
    mainwindow.h \
    symulator.h
    RegulatorPID.h
    WartoscZadana.h
    ModelARX.h

FORMS += \
    dialog_arx.ui \
    dialog_sieciowy.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
