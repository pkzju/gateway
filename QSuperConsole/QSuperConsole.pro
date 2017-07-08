

QT += core gui printsupport

QT += widgets printsupport serialport serialbus network axcontainer

CONFIG += c++11


#########  QFRAMER  #########
include(../qframer/qframer.pri)
INCLUDEPATH += $$QFRAMER_INCLUDEPATH
LIBS += -L$${QFRAMER_LIBPATH} $${QFRAMER_LIBS}

#########  USERUI  #########
include(../userui/userui.pri)
INCLUDEPATH += $$USERUI_INCLUDEPATH
LIBS += -L$${USERUI_LIBPATH} $${USERUI_LIBS}

# application name
TARGET = SuperConsole

# type
TEMPLATE = app
DEPENDPATH += .

# build dir
BuildDir =build_$$QT_VERSION

CONFIG(debug, debug|release) {
    DESTDIR     = $$OUT_PWD/debug/$$BuildDir
    OBJECTS_DIR = $$OUT_PWD/debug/$$BuildDir/.obj
    MOC_DIR     = $$OUT_PWD/debug/$$BuildDir/.moc
    RCC_DIR     = $$OUT_PWD/debug/$$BuildDir/.rcc
    UI_DIR      = $$OUT_PWD/debug/$$BuildDir/.ui
} else {
    DESTDIR     = $$OUT_PWD/release/$$BuildDir
    OBJECTS_DIR = $$OUT_PWD/release/$$BuildDir/.obj
    MOC_DIR     = $$OUT_PWD/release/$$BuildDir/.moc
    RCC_DIR     = $$OUT_PWD/release/$$BuildDir/.rcc
    UI_DIR      = $$OUT_PWD/release/$$BuildDir/.ui
}

SOURCES += \
    dialogs/aboutdialog.cpp \
    dialogs/bgskinpopup.cpp \
    dialogs/settingdialog.cpp \
    functionpages/rightfloatwindow.cpp \
    mainwindow/centerwindow.cpp \
    mainwindow/mainwindow.cpp \
    mainwindow/settingmenu.cpp \
    mainwindow/settingmenucontroller.cpp \
    mainwindow/thememenu.cpp \
    main.cpp \
    dialogs/logindialog.cpp


HEADERS  += \
    dialogs/aboutdialog.h \
    dialogs/bgskinpopup.h \
    dialogs/settingdialog.h \
    functionpages/rightfloatwindow.h \
    mainwindow/centerwindow.h \
    mainwindow/mainwindow.h \
    mainwindow/settingmenu.h \
    mainwindow/settingmenucontroller.h \
    mainwindow/thememenu.h \
    dialogs/logindialog.h


RESOURCES += \
    QSuperConsole.qrc

# QT5 applciation icon
contains(QT_MAJOR_VERSION, 5){
    RC_ICONS = "skin/images/ico.ico"
}

qtHaveModule(QWebView): QT += QWebView

win32:LIBS += -L$$PWD\lib\ -lControlCAN

CONFIG(debug, debug|release) {

}else {

}

INCLUDEPATH += "./canopen"

# Copies the given files to the destination directory
defineTest(copyFiles) {
    files = $$1
    DDIR = $$2

    !exists($$DDIR){
        mkpath($$DDIR)
    }
    for(FILE, files) {
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g
        QMAKE_PRE_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(QMAKE_PRE_LINK)
}

copyFiles($$PWD/../userui/json, $$DESTDIR/json/)
copyFiles($$PWD/lib, $$DESTDIR/lib/)
