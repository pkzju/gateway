include(excelbase.pri)


QT += axcontainer

INCLUDEPATH += .
TEMPLATE = lib
TARGET = excelbase

DEFINES += EXCELBASE_BUILD
CONFIG += $${EXCELBASE_LIBRARY_TYPE} create_prl
VERSION = $${EXCELBASE_VERSION}

# build dir
BuildDir =build_$$QT_VERSION

DESTDIR = $${EXCELBASE_LIBPATH}
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
    $$PWD/ExcelBase.h 

SOURCES += \
    $$PWD/ExcelBase.cpp 

DISTFILES +=


