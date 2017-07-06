QFRAMER_VERSION = 1.1.0

isEmpty(QFRAMER_LIBRARY_TYPE) {
    QFRAMER_LIBRARY_TYPE = staticlib
}

QFRAMER_INCLUDEPATH = .
QFRAMER_LIBS = -lqframer
contains(QFRAMER_LIBRARY_TYPE, staticlib) {
    DEFINES += QFRAMER_STATIC
} else {
    DEFINES += QFRAMER_SHARED
    win32:QFRAMER_LIBS = -lqframer1
}

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
TARGET = qframer

DEFINES += QFRAMER_BUILD
CONFIG += $${QFRAMER_LIBRARY_TYPE}
VERSION = $${QFRAMER_VERSION}

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
headers.path = $${PREFIX}/include/qframer
private_headers.files = $${PRIVATE_HEADERS}
private_headers.path = $${PREFIX}/include/qframer/private
target.path = $${PREFIX}/$${LIBDIR}
INSTALLS += headers  private_headers target

# pkg-config support
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(QFRAMER_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DQFRAMER_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DQFRAMER_SHARED
}
unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl


QT += core gui printsupport


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

