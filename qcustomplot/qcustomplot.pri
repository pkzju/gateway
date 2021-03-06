QCUSTOMPLOT_VERSION = 1.1.0

isEmpty(QCUSTOMPLOT_LIBRARY_TYPE) {
    QCUSTOMPLOT_LIBRARY_TYPE = staticlib
}

QCUSTOMPLOT_INCLUDEPATH = $$PWD
QCUSTOMPLOT_DIR = $$PWD
QCUSTOMPLOT_LIBS = -lqcustomplot
contains(QCUSTOMPLOT_LIBRARY_TYPE, staticlib) {
    DEFINES += QCUSTOMPLOT_STATIC
} else {
    DEFINES += QCUSTOMPLOT_SHARED
    win32:QCUSTOMPLOT_LIBS = -lqcustomplot1
}

QCUSTOMPLOT_LIBPATH = $$PWD/lib
CONFIG(debug, debug|release) {
    QCUSTOMPLOT_LIBPATH = $$PWD/lib/debug
} else {
    QCUSTOMPLOT_LIBPATH = $$PWD/lib/release
}
