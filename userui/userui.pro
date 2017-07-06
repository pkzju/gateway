USERUI_VERSION = 1.1.0

QT += widgets printsupport serialport serialbus network axcontainer

isEmpty(USERUI_LIBRARY_TYPE) {
    USERUI_LIBRARY_TYPE = staticlib
}

USERUI_INCLUDEPATH = .
USERUI_LIBS = -luserui
contains(USERUI_LIBRARY_TYPE, staticlib) {
    DEFINES += USERUI_STATIC
} else {
    DEFINES += USERUI_SHARED
    win32:USERUI_LIBS = -luserui1
}

USERUI_LIBPATH = $${USERUI_INCLUDEPATH}/lib
USERUI_NEEDLIBS = -lControlCAN
LIBS += -L$${USERUI_LIBPATH} $${USERUI_NEEDLIBS}

isEmpty(PREFIX) {
    unix {
        PREFIX = /usr
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}
isEmpty(LIBDIR) {
    LIBDIR = lib
}


INCLUDEPATH += .
TEMPLATE = lib
TARGET = USERUI

DEFINES += USERUI_BUILD
CONFIG += $${USERUI_LIBRARY_TYPE}
VERSION = $${USERUI_VERSION}


#########  canopen  #########
CANOPEN_VERSION = 1.1.0

isEmpty(CANOPEN_LIBRARY_TYPE) {
    CANOPEN_LIBRARY_TYPE = staticlib
}

CANOPEN_INCLUDEPATH = ../canopen
CANOPEN_LIBPATH = $${CANOPEN_INCLUDEPATH}/lib
CANOPEN_LIBS = -lcanopen

contains(CANOPEN_LIBRARY_TYPE, staticlib) {
    DEFINES += CANOPEN_STATIC
} else {
    DEFINES += CANOPEN_SHARED
    #win32:CANOPEN_LIBS = -lqjsonrpc1
}

CONFIG(debug, debug|release) {
    CANOPEN_LIBPATH = $${CANOPEN_LIBPATH}/debug
}else{
    CANOPEN_LIBPATH = $${CANOPEN_LIBPATH}/release
}

LIBS += -L$${CANOPEN_LIBPATH} $${CANOPEN_LIBS}

INCLUDEPATH += $${CANOPEN_INCLUDEPATH}

unix:!macx:QMAKE_RPATHDIR += $${CANOPEN_LIBPATH}
macx {
    QMAKE_RPATHDIR += $${CANOPEN_LIBPATH}
    QMAKE_LFLAGS += -Wl,-rpath,$${CANOPEN_LIBPATH}
}


#########  excelbase  #########
EXCELBASE_VERSION = 1.1.0

isEmpty(EXCELBASE_LIBRARY_TYPE) {
    EXCELBASE_LIBRARY_TYPE = staticlib
}

EXCELBASE_INCLUDEPATH = ../excelbase
EXCELBASE_LIBPATH = $${EXCELBASE_INCLUDEPATH}/lib
EXCELBASE_LIBS = -lexcelbase

contains(EXCELBASE_LIBRARY_TYPE, staticlib) {
    DEFINES += EXCELBASE_STATIC
} else {
    DEFINES += EXCELBASE_SHARED
}

CONFIG(debug, debug|release) {
    EXCELBASE_LIBPATH = $${EXCELBASE_LIBPATH}/debug
}else{
    EXCELBASE_LIBPATH = $${EXCELBASE_LIBPATH}/release
}

LIBS += -L$${EXCELBASE_LIBPATH} $${EXCELBASE_LIBS}

INCLUDEPATH += $${EXCELBASE_INCLUDEPATH}

unix:!macx:QMAKE_RPATHDIR += $${EXCELBASE_LIBPATH}
macx {
    QMAKE_RPATHDIR += $${EXCELBASE_LIBPATH}
    QMAKE_LFLAGS += -Wl,-rpath,$${EXCELBASE_LIBPATH}
}

#########  QCUSTOMPLOT  #########
QCUSTOMPLOT_VERSION = 1.1.0

isEmpty(QCUSTOMPLOT_LIBRARY_TYPE) {
    QCUSTOMPLOT_LIBRARY_TYPE = staticlib
}

QCUSTOMPLOT_INCLUDEPATH = ../qcustomplot
QCUSTOMPLOT_LIBPATH = $${QCUSTOMPLOT_INCLUDEPATH}/lib
QCUSTOMPLOT_LIBS = -lqcustomplot

contains(QCUSTOMPLOT_LIBRARY_TYPE, staticlib) {
    DEFINES += QCUSTOMPLOT_STATIC
} else {
    DEFINES += QCUSTOMPLOT_SHARED
}

CONFIG(debug, debug|release) {
    QCUSTOMPLOT_LIBPATH = $${QCUSTOMPLOT_LIBPATH}/debug
}else{
    QCUSTOMPLOT_LIBPATH = $${QCUSTOMPLOT_LIBPATH}/release
}

LIBS += -L$${QCUSTOMPLOT_LIBPATH} $${QCUSTOMPLOT_LIBS}

INCLUDEPATH += $${QCUSTOMPLOT_INCLUDEPATH}

unix:!macx:QMAKE_RPATHDIR += $${QCUSTOMPLOT_LIBPATH}
macx {
    QMAKE_RPATHDIR += $${QCUSTOMPLOT_LIBPATH}
    QMAKE_LFLAGS += -Wl,-rpath,$${QCUSTOMPLOT_LIBPATH}
}


# build dir
BuildDir =build_$$QT_VERSION

CONFIG(debug, debug|release) {
    DESTDIR = $$OUT_PWD/lib/debug
    OBJECTS_DIR = $$OUT_PWD/debug/$$BuildDir/.obj
    MOC_DIR = $$OUT_PWD/debug/$$BuildDir/.moc
    RCC_DIR = $$OUT_PWD/debug/$$BuildDir/.rcc
    UI_DIR = $$OUT_PWD/debug/$$BuildDir/.ui
} else {
    DESTDIR = $$OUT_PWD/lib/release
    OBJECTS_DIR = $$OUT_PWD/release/$$BuildDir/.obj
    MOC_DIR = $$OUT_PWD/release/$$BuildDir/.moc
    RCC_DIR = $$OUT_PWD/release/$$BuildDir/.rcc
    UI_DIR = $$OUT_PWD/release/$$BuildDir/.ui
}

#win32:DESTDIR = $$OUT_PWD
macx:QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/

# install
headers.files = $${INSTALL_HEADERS}
headers.path = $${PREFIX}/include/userui
private_headers.files = $${PRIVATE_HEADERS}
private_headers.path = $${PREFIX}/include/userui/private
target.path = $${PREFIX}/$${LIBDIR}
INSTALLS += headers  private_headers target

# pkg-config support
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(USERUI_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DUSERUI_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DUSERUI_SHARED
}
unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl



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

