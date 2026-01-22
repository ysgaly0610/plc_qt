QT       += core gui
#QT       += axWidget
#QT       += axWidget_2
QT       += axcontainer    # QaxContainer模块是访问ActiveX控件和COM对象的一个Windows扩展

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_FILE = picture.rc
#CONFIG += debug

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IniFile.cpp \
    actprogtypelib.cpp \
    actutltypelib.cpp \
    changepassworddlg.cpp \
    config.cpp \
    main.cpp \
    mainwindow.cpp \
    passworddlg.cpp \
    plcthread.cpp \
    setting.cpp \
    setupdialog.cpp \
    soapC.cpp \
    soapServer.cpp \
    soapServerLib.cpp \
    stdsoap2.cpp \
    thread.cpp \
    tinystr.cpp \
    tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp

HEADERS += \
    IniFile.h \
    actprogtypelib.h \
    actutltypelib.h \
    changepassworddlg.h \
    config.h \
    iMesImage.h \
    iMesImage.nsmap \
    mainwindow.h \
    passworddlg.h \
    plcthread.h \
    setting.h \
    setupdialog.h \
    soapH.h \
    soapStub.h \
    stdsoap2.h \
    thread.h \
    tinystr.h \
    tinyxml.h

FORMS += \
    changepassworddlg.ui \
    mainwindow.ui \
    passworddlg.ui \
    setupdialog.ui

TRANSLATIONS += \
    PLC_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    ../QtWebApp/QtWebApp/QtWebApp.pro

LIBS += -lpthread libwsock32 libws2_32

