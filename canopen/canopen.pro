CANOPEN_VERSION = 1.1.0

isEmpty(CANOPEN_LIBRARY_TYPE) {
    CANOPEN_LIBRARY_TYPE = staticlib
}

CANOPEN_INCLUDEPATH = .
CANOPEN_LIBS = -lcanopen
contains(CANOPEN_LIBRARY_TYPE, staticlib) {
    DEFINES += CANOPEN_STATIC
} else {
    DEFINES += CANOPEN_SHARED
    win32:CANOPEN_LIBS = -lcanopen1
}

CANOPEN_LIBPATH = $${CANOPEN_INCLUDEPATH}/lib
CANOPEN_NEEDLIBS = -lControlCAN
LIBS += -L$${CANOPEN_LIBPATH} $${CANOPEN_NEEDLIBS}

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
TARGET = canopen

DEFINES += CANOPEN_BUILD
CONFIG += $${CANOPEN_LIBRARY_TYPE}
VERSION = $${CANOPEN_VERSION}

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
headers.path = $${PREFIX}/include/canopen
private_headers.files = $${PRIVATE_HEADERS}
private_headers.path = $${PREFIX}/include/canopen/private
target.path = $${PREFIX}/$${LIBDIR}
INSTALLS += headers  private_headers target

# pkg-config support
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(CANOPEN_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DCANOPEN_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DCANOPEN_SHARED
}
unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl

HEADERS += \
    $$PWD/applicfg.h \
    $$PWD/can.h \
    $$PWD/can_driver.h \
    $$PWD/canfestival.h \
    $$PWD/data.h \
    $$PWD/dcf.h \
    $$PWD/def.h \
    $$PWD/emcy.h \
    $$PWD/lifegrd.h \
    $$PWD/lss.h \
    $$PWD/nmtMaster.h \
    $$PWD/nmtSlave.h \
    $$PWD/objacces.h \
    $$PWD/objdictdef.h \
    $$PWD/pdo.h \
    $$PWD/sdo.h \
    $$PWD/states.h \
    $$PWD/sync.h \
    $$PWD/sysdep.h \
    $$PWD/timer.h \
    $$PWD/timers_driver.h \
    $$PWD/timerscfg.h \
    $$PWD/ControlCAN.H \
    $$PWD/config.h \
    $$PWD/master.h

SOURCES += \
    $$PWD/canfestival.c \
    $$PWD/dcf.c \
    $$PWD/emcy.c \
    $$PWD/lifegrd.c \
    $$PWD/lss.c \
    $$PWD/nmtMaster.c \
    $$PWD/nmtSlave.c \
    $$PWD/objacces.c \
    $$PWD/pdo.c \
    $$PWD/sdo.c \
    $$PWD/states.c \
    $$PWD/sync.c \
    $$PWD/timer.c \
    $$PWD/timers_win32.c \
    $$PWD/master.c \
    $$PWD/symbols.c


DISTFILES +=


