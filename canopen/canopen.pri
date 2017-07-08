CANOPEN_VERSION = 1.1.0

isEmpty(CANOPEN_LIBRARY_TYPE) {
    CANOPEN_LIBRARY_TYPE = staticlib
}

CANOPEN_INCLUDEPATH = $$PWD
CANOPEN_DIR = $$PWD
CANOPEN_LIBS = -lcanopen
contains(CANOPEN_LIBRARY_TYPE, staticlib) {
    DEFINES += CANOPEN_STATIC
} else {
    DEFINES += CANOPEN_SHARED
    win32:CANOPEN_LIBS = -lcanopen1
}

CANOPEN_LIBPATH = $$PWD/lib
CONFIG(debug, debug|release) {
    CANOPEN_LIBPATH = $$PWD/lib/debug
} else {
    CANOPEN_LIBPATH = $$PWD/lib/release
}


