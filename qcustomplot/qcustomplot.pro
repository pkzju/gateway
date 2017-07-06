QCUSTOMPLOT_VERSION = 1.1.0

isEmpty(QCUSTOMPLOT_LIBRARY_TYPE) {
    QCUSTOMPLOT_LIBRARY_TYPE = staticlib
}

QCUSTOMPLOT_INCLUDEPATH = .
QCUSTOMPLOT_LIBS = -lqcustomplot
contains(QCUSTOMPLOT_LIBRARY_TYPE, staticlib) {
    DEFINES += QCUSTOMPLOT_STATIC
} else {
    DEFINES += QCUSTOMPLOT_SHARED
    win32:QCUSTOMPLOT_LIBS = -lqcustomplot1
}

QCUSTOMPLOT_LIBPATH = $${QCUSTOMPLOT_INCLUDEPATH}/lib
QCUSTOMPLOT_NEEDLIBS = -lControlCAN
LIBS += -L$${QCUSTOMPLOT_LIBPATH} $${QCUSTOMPLOT_NEEDLIBS}

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
TARGET = qcustomplot

DEFINES += QCUSTOMPLOT_BUILD
CONFIG += $${QCUSTOMPLOT_LIBRARY_TYPE}
VERSION = $${QCUSTOMPLOT_VERSION}

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
headers.path = $${PREFIX}/include/qcustomplot
private_headers.files = $${PRIVATE_HEADERS}
private_headers.path = $${PREFIX}/include/qcustomplot/private
target.path = $${PREFIX}/$${LIBDIR}
INSTALLS += headers  private_headers target

# pkg-config support
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(QCUSTOMPLOT_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DQCUSTOMPLOT_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DQCUSTOMPLOT_SHARED
}
unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl



HEADERS += \
    $$PWD/qcustomplot.h

SOURCES += \
    $$PWD/qcustomplot.cpp 

DISTFILES +=


