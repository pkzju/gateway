include(userui.pri)

QT += widgets printsupport serialport serialbus network axcontainer


INCLUDEPATH += $$PWD
TEMPLATE = lib
TARGET = userui

DEFINES += USERUI_BUILD
CONFIG += $${USERUI_LIBRARY_TYPE} create_prl link_prl
VERSION = $${USERUI_VERSION}


#########  CANOPEN  #########
include(../canopen/canopen.pri)
INCLUDEPATH += $$CANOPEN_INCLUDEPATH
LIBS += -L$${CANOPEN_LIBPATH} $${CANOPEN_LIBS}

#########  EXCELBASE  #########
include(../excelbase/excelbase.pri)
INCLUDEPATH += $$EXCELBASE_INCLUDEPATH
LIBS += -L$${EXCELBASE_LIBPATH} $${EXCELBASE_LIBS}

#########  QCUSTOMPLOT  #########
include(../qcustomplot/qcustomplot.pri)
INCLUDEPATH += $$QCUSTOMPLOT_INCLUDEPATH
LIBS += -L$${QCUSTOMPLOT_LIBPATH} $${QCUSTOMPLOT_LIBS}

# build dir
BuildDir =build_$$QT_VERSION

DESTDIR = $${USERUI_LIBPATH}
CONFIG(debug, debug|release) {
    OBJECTS_DIR = $$OUT_PWD/debug/$$BuildDir/.obj
    MOC_DIR = $$OUT_PWD/debug/$$BuildDir/.moc
    RCC_DIR = $$OUT_PWD/debug/$$BuildDir/.rcc
    UI_DIR = $$OUT_PWD/debug/$$BuildDir/.ui
} else {
    OBJECTS_DIR = $$OUT_PWD/release/$$BuildDir/.obj
    MOC_DIR = $$OUT_PWD/release/$$BuildDir/.moc
    RCC_DIR = $$OUT_PWD/release/$$BuildDir/.rcc
    UI_DIR = $$OUT_PWD/release/$$BuildDir/.ui
}

FORMS += \
    $$PWD/canui.ui \
    $$PWD/serialportui.ui \
    $$PWD/serialportsettingsdialog.ui \
    $$PWD/modbusui.ui \
    $$PWD/loginframe.ui \
    $$PWD/tcpclientframe.ui \
    $$PWD/tcpserverframe.ui \
    $$PWD/tcpportsettings.ui \
    $$PWD/groupframe.ui \
    $$PWD/fanhomeframe.ui \
    $$PWD/canevs.ui \
    $$PWD/cmotormodbus.ui \
    $$PWD/newhome.ui


HEADERS += \
    $$PWD/canui.h \
    $$PWD/serialportui.h \
    $$PWD/serialportsettingsdialog.h \
    $$PWD/modbusui.h \
    $$PWD/writeregistermodel.h \
    $$PWD/loginframe.h \
    $$PWD/tcpclientframe.h \
    $$PWD/tcpserverframe.h \
    $$PWD/tcpportsettings.h \
    $$PWD/groupframe.h \
    $$PWD/fanhomeframe.h \
    $$PWD/canevs.h \
    $$PWD/cmotormodbus.h \
    $$PWD/newhome.h\
    $$PWD/lamp/qcw_indicatorlamp.h \
    $$PWD/thread/canthread.h \
    $$PWD/thread/serialportthread.h \
    $$PWD/fanmotor/fanmotor.h \
    $$PWD/fanmotor/qmotor.h \
    $$PWD/fanmotor/fpublic.h \
    $$PWD/qusbcan/pusbcan.h \
    $$PWD/qusbcan/qusbcan.h \
    $$PWD/ctools/ctools.h \
    $$PWD/ctools/modbushandler.h \
    $$PWD/ctools/serialportsettings.h \
    $$PWD/fanmotor/canmotor.h


SOURCES += \
    $$PWD/canui.cpp \
    $$PWD/serialportui.cpp \
    $$PWD/serialportsettingsdialog.cpp \
    $$PWD/modbusui.cpp \
    $$PWD/writeregistermodel.cpp \
    $$PWD/loginframe.cpp \
    $$PWD/tcpclientframe.cpp \
    $$PWD/tcpserverframe.cpp \
    $$PWD/tcpportsettings.cpp \
    $$PWD/groupframe.cpp \
    $$PWD/fanhomeframe.cpp \
    $$PWD/canevs.cpp \
    $$PWD/cmotormodbus.cpp \
    $$PWD/newhome.cpp\
    $$PWD/lamp/qcw_indicatorlamp.cpp \
    $$PWD/thread/canthread.cpp \
    $$PWD/thread/serialportthread.cpp \
    $$PWD/fanmotor/qmotor.cpp \
    $$PWD/qusbcan/pusbcan.cpp \
    $$PWD/qusbcan/qusbcan.cpp \
    $$PWD/ctools/ctools.cpp \
    $$PWD/ctools/modbushandler.cpp \
    $$PWD/ctools/serialportsettings.cpp \
    $$PWD/fanmotor/fpublic.cpp

DISTFILES +=

