include(qframer.pri)

QT += core gui printsupport

INCLUDEPATH += $$PWD
TEMPLATE = lib
TARGET = qframer

DEFINES += QFRAMER_BUILD
CONFIG += $${QFRAMER_LIBRARY_TYPE} create_prl
VERSION = $${QFRAMER_VERSION}

# build dir
BuildDir =build_$$QT_VERSION

DESTDIR = $${QFRAMER_LIBPATH}
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


HEADERS += \
    $$PWD//fcenterwindow.h \
    $$PWD//fmainwindow.h \
    $$PWD//fnavgationbar.h \
    $$PWD//ftabwidget.h \
    $$PWD//ftitlebar.h \
    $$PWD//futil.h \
    $$PWD/ftoolbutton.h \
    $$PWD/fcheckablebutton.h \
    $$PWD/fbasedialog.h \
    $$PWD/fbasepushbutton.h \
    $$PWD/flywidget.h \
    $$PWD/fmovablewidget.h \
    $$PWD/fshadowlabel.h \
    $$PWD/fsubwindown.h

SOURCES += \
    $$PWD//fcenterwindow.cpp \
    $$PWD//fmainwindow.cpp \
    $$PWD//fnavgationbar.cpp \
    $$PWD//ftabwidget.cpp \
    $$PWD//ftitlebar.cpp \
    $$PWD//futil.cpp \
    $$PWD/ftoolbutton.cpp \
    $$PWD/fcheckablebutton.cpp \
    $$PWD/fbasedialog.cpp \
    $$PWD/fbasepushbutton.cpp \
    $$PWD/flywidget.cpp \
    $$PWD/fmovablewidget.cpp \
    $$PWD/fshadowlabel.cpp \
    $$PWD/fsubwindown.cpp

