USERUI_VERSION = 1.1.0

isEmpty(USERUI_LIBRARY_TYPE) {
    USERUI_LIBRARY_TYPE = staticlib
}

USERUI_INCLUDEPATH = $$PWD
USERUI_DIR = $$PWD
USERUI_LIBS = -luserui
contains(USERUI_LIBRARY_TYPE, staticlib) {
    DEFINES += USERUI_STATIC
} else {
    DEFINES += USERUI_SHARED
    win32:USERUI_LIBS = -luserui1
}

USERUI_LIBPATH = $$PWD/lib
CONFIG(debug, debug|release) {
    USERUI_LIBPATH = $$PWD/lib/debug
} else {
    USERUI_LIBPATH = $$PWD/lib/release
}

