QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    autoinstalls.cpp \
    exeapp.cpp \
    findtools.cpp \
    main.cpp \
    mainwindow.cpp \
    mpm.cpp \
    other_mainwindow.cpp \
    setting_corl.cpp \
    windows_function.cpp

HEADERS += \
    autoinstalls.h \
    exeapp.h \
    findtools.h \
    mainwindow.h \
    mpm.h \
    other_mainwindow.h \
    setting_corl.h \
    windows_function.h

FORMS += \
    mainwindow.ui \
    other_mainwindow.ui

TRANSLATIONS += \
    0-1_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
src.qrc
#Icon.qrc
#RC_FILE = icon.rc
##Window API LIB
#pragma (lib, "Setupapi.lib" )
#pragma (lib, "PowrProf.lib" )
#pragma (lib,"Cfgmgr32.lib")
#LIBS +=Setupapi.lib
#LIBS +=PowrProf.lib
#LIBS +=Cfgmgr32.lib


unix|win32: LIBS += -lSetupAPI

unix|win32: LIBS += -lpowrprof

unix|win32: LIBS += -lOle32

#win32: LIBS += -lAdvAPI32 	User32.lib

VERSION = 2.0.2.4

RC_ICONS = app.ico

QMAKE_TARGET_COMPANY ="Quantacn.com"

QMAKE_TARGET_DESCRIPTION = "HWQA_TEST APP"

QMAKE_TARGET_COPYRIGHT = "Shangjiang.Li@Quantacn.com"

QMAKE_TARGET_PRODUCT = "QT app"

RC_LANG = 0x0800

