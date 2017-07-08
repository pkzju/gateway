EXCELBASE_VERSION = 1.1.0

isEmpty(EXCELBASE_LIBRARY_TYPE) {
    EXCELBASE_LIBRARY_TYPE = staticlib
}

EXCELBASE_INCLUDEPATH = $$PWD
EXCELBASE_DIR = $$PWD
EXCELBASE_LIBS = -lexcelbase
contains(EXCELBASE_LIBRARY_TYPE, staticlib) {
    DEFINES += EXCELBASE_STATIC
} else {
    DEFINES += EXCELBASE_SHARED
    win32:EXCELBASE_LIBS = -lexcelbase1
}

EXCELBASE_LIBPATH = $$PWD/lib
CONFIG(debug, debug|release) {
    EXCELBASE_LIBPATH = $$PWD/lib/debug
} else {
    EXCELBASE_LIBPATH = $$PWD/lib/release
}






