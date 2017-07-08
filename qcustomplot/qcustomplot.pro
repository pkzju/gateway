include(qcustomplot.pri)

INCLUDEPATH += .
TEMPLATE = lib
TARGET = qcustomplot

DEFINES += QCUSTOMPLOT_BUILD
CONFIG += $${QCUSTOMPLOT_LIBRARY_TYPE} create_prl
VERSION = $${QCUSTOMPLOT_VERSION}

# build dir
BuildDir =build_$$QT_VERSION

DESTDIR = $${QCUSTOMPLOT_LIBPATH}
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
    $$PWD/qcustomplot.h

SOURCES += \
    $$PWD/qcustomplot.cpp 

DISTFILES +=


