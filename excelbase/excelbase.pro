EXCELBASE_VERSION = 1.1.0

QT += axcontainer


isEmpty(EXCELBASE_LIBRARY_TYPE) {
    EXCELBASE_LIBRARY_TYPE = staticlib
}

EXCELBASE_INCLUDEPATH = .
EXCELBASE_LIBS = -lexcelbase
contains(EXCELBASE_LIBRARY_TYPE, staticlib) {
    DEFINES += EXCELBASE_STATIC
} else {
    DEFINES += EXCELBASE_SHARED
    win32:EXCELBASE_LIBS = -lexcelbase1
}

EXCELBASE_LIBPATH = $${EXCELBASE_INCLUDEPATH}/lib
EXCELBASE_NEEDLIBS = -lControlCAN
LIBS += -L$${EXCELBASE_LIBPATH} $${EXCELBASE_NEEDLIBS}

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
TARGET = excelbase

DEFINES += EXCELBASE_BUILD
CONFIG += $${EXCELBASE_LIBRARY_TYPE}
VERSION = $${EXCELBASE_VERSION}

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
headers.path = $${PREFIX}/include/excelbase
private_headers.files = $${PRIVATE_HEADERS}
private_headers.path = $${PREFIX}/include/excelbase/private
target.path = $${PREFIX}/$${LIBDIR}
INSTALLS += headers  private_headers target

# pkg-config support
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(EXCELBASE_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DEXCELBASE_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DEXCELBASE_SHARED
}
unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl


HEADERS += \
    $$PWD/ExcelBase.h 

SOURCES += \
    $$PWD/ExcelBase.cpp 

DISTFILES +=


