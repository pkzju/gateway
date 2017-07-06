
TARGET   = treefrog
TEMPLATE = lib
CONFIG  += core shared console c++11
CONFIG  -= lib_bundle
QT      += sql network xml
DEFINES += TF_MAKEDLL
INCLUDEPATH += ../include
INCLUDEPATH += .
DEPENDPATH  += ../include
DEFINES += CANOPEN_BUILD
CONFIG += $${CANOPEN_LIBRARY_TYPE}
TF_VERSION = 1.17.0
include(../tfbase.pri)
include(../include/headers.pri)
VERSION = $$TF_VERSION

windows:CONFIG(debug, debug|release) {
  TARGET = $$join(TARGET,,,d)
}

isEmpty(TF_LIBRARY_TYPE) {
    CANOPEN_LIBRARY_TYPE = staticlib
}

TF_INCLUDEPATH = .
TF_LIBS = -ltreefrog
contains(TF_LIBRARY_TYPE, staticlib) {
    DEFINES += TF_STATIC
} else {
    DEFINES += TF_SHARED
    win32:CANOPEN_LIBS = -ltreefrog1
}

TF_LIBPATH = $${TF_INCLUDEPATH}/lib
LIBS += -L$${TF_LIBPATH} $${TF_NEEDLIBS}

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
headers.path = $${PREFIX}/include/treefrog
private_headers.files = $${PRIVATE_HEADERS}
private_headers.path = $${PREFIX}/include/treefrog/private
target.path = $${PREFIX}/$${LIBDIR}
INSTALLS += headers  private_headers target

# pkg-config support
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
equals(TF_LIBRARY_TYPE, staticlib) {
    QMAKE_PKGCONFIG_CFLAGS = -DTF_STATIC
} else {
    QMAKE_PKGCONFIG_CFLAGS = -DTF_SHARED
}
unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl

HEADERS += twebapplication.h
SOURCES += twebapplication.cpp
HEADERS += tapplicationserverbase.h
SOURCES += tapplicationserverbase.cpp
HEADERS += tthreadapplicationserver.h
SOURCES += tthreadapplicationserver.cpp
HEADERS += tactioncontext.h
SOURCES += tactioncontext.cpp
HEADERS += tdatabasecontext.h
SOURCES += tdatabasecontext.cpp
HEADERS += tactionthread.h
SOURCES += tactionthread.cpp
#HEADERS += tactionforkprocess.h
#SOURCES += tactionforkprocess.cpp
HEADERS += thttpsocket.h
SOURCES += thttpsocket.cpp
#HEADERS += thttp2socket.h
#SOURCES += thttp2socket.cpp
#HEADERS += thttpbuffer.h
#SOURCES += thttpbuffer.cpp
HEADERS += tsendbuffer.h
SOURCES += tsendbuffer.cpp
HEADERS += tabstractcontroller.h
SOURCES += tabstractcontroller.cpp
HEADERS += tactioncontroller.h
SOURCES += tactioncontroller.cpp
HEADERS += directcontroller.h
SOURCES += directcontroller.cpp
HEADERS += tactionview.h
SOURCES += tactionview.cpp
HEADERS += tactionmailer.h
SOURCES += tactionmailer.cpp
HEADERS += tsqldatabasepool.h
SOURCES += tsqldatabasepool.cpp
HEADERS += tsqlobject.h
SOURCES += tsqlobject.cpp
HEADERS += tsqlormapperiterator.h
SOURCES += tsqlormapperiterator.cpp
HEADERS += tsqlquery.h
SOURCES += tsqlquery.cpp
HEADERS += tsqlqueryormapper.h
SOURCES += tsqlqueryormapper.cpp
HEADERS += tsqlqueryormapperiterator.h
SOURCES += tsqlqueryormapperiterator.cpp
HEADERS += tsqltransaction.h
SOURCES += tsqltransaction.cpp
HEADERS += tcriteria.h
SOURCES += tcriteria.cpp
HEADERS += tcriteriaconverter.h
SOURCES += tcriteriaconverter.cpp
HEADERS += thttprequest.h
SOURCES += thttprequest.cpp
HEADERS += thttpresponse.h
SOURCES += thttpresponse.cpp
HEADERS += tmultipartformdata.h
SOURCES += tmultipartformdata.cpp
HEADERS += tcontentheader.h
SOURCES += tcontentheader.cpp
HEADERS += thttputility.h
SOURCES += thttputility.cpp
HEADERS += thtmlattribute.h
SOURCES += thtmlattribute.cpp
HEADERS += ttextview.h
SOURCES += ttextview.cpp
HEADERS += tdirectview.h
SOURCES += tdirectview.cpp
HEADERS += tactionhelper.h
SOURCES += tactionhelper.cpp
HEADERS += tviewhelper.h
SOURCES += tviewhelper.cpp
HEADERS += tprototypeajaxhelper.h
SOURCES += tprototypeajaxhelper.cpp
HEADERS += toption.h
SOURCES += toption.cpp
HEADERS += ttemporaryfile.h
SOURCES += ttemporaryfile.cpp
HEADERS += tcookiejar.h
SOURCES += tcookiejar.cpp
HEADERS += tsession.h
SOURCES += tsession.cpp
HEADERS += tsessionmanager.h
SOURCES += tsessionmanager.cpp
HEADERS += tsessionstore.h
SOURCES += tsessionstore.cpp
HEADERS += tsessionstorefactory.h
SOURCES += tsessionstorefactory.cpp
HEADERS += tsessionsqlobjectstore.h
SOURCES += tsessionsqlobjectstore.cpp
HEADERS += tsessioncookiestore.h
SOURCES += tsessioncookiestore.cpp
HEADERS += tsessionfilestore.h
SOURCES += tsessionfilestore.cpp
HEADERS += tsessionredisstore.h
SOURCES += tsessionredisstore.cpp
HEADERS += thtmlparser.h
SOURCES += thtmlparser.cpp
HEADERS += tabstractmodel.h
SOURCES += tabstractmodel.cpp
HEADERS += tmodelutil.h
#SOURCES += tmodelutil.cpp
HEADERS += tmodelobject.h
SOURCES += tmodelobject.cpp
HEADERS += tsystemglobal.h
SOURCES += tsystemglobal.cpp
HEADERS += tglobal.h
SOURCES += tglobal.cpp
HEADERS += taccesslog.h
SOURCES += taccesslog.cpp
HEADERS += taccesslogstream.h
SOURCES += taccesslogstream.cpp
HEADERS += tlog.h
SOURCES += tlog.cpp
HEADERS += tlogger.h
SOURCES += tlogger.cpp
HEADERS += tloggerfactory.h
SOURCES += tloggerfactory.cpp
HEADERS += tfilelogger.h
SOURCES += tfilelogger.cpp
HEADERS += tabstractlogstream.h
SOURCES += tabstractlogstream.cpp
HEADERS += tsharedmemorylogstream.h
SOURCES += tsharedmemorylogstream.cpp
HEADERS += tbasiclogstream.h
SOURCES += tbasiclogstream.cpp
#HEADERS += tmailerfactory.h
#SOURCES += tmailerfactory.cpp
HEADERS += tmailmessage.h
SOURCES += tmailmessage.cpp
HEADERS += tsmtpmailer.h
SOURCES += tsmtpmailer.cpp
HEADERS += tpopmailer.h
SOURCES += tpopmailer.cpp
HEADERS += tsendmailmailer.h
SOURCES += tsendmailmailer.cpp
HEADERS += tcryptmac.h
SOURCES += tcryptmac.cpp
HEADERS += tinternetmessageheader.h
SOURCES += tinternetmessageheader.cpp
HEADERS += thttpheader.h
SOURCES += thttpheader.cpp
HEADERS += turlroute.h
SOURCES += turlroute.cpp
HEADERS += tabstractuser.h
SOURCES += tabstractuser.cpp
HEADERS += tformvalidator.h
SOURCES += tformvalidator.cpp
HEADERS += taccessvalidator.h
SOURCES += taccessvalidator.cpp
HEADERS += tpaginator.h
SOURCES += tpaginator.cpp
HEADERS += tkvsdatabase.h
SOURCES += tkvsdatabase.cpp
HEADERS += tkvsdatabasepool.h
SOURCES += tkvsdatabasepool.cpp
HEADERS += tkvsdriver.h
SOURCES += tkvsdriver.cpp
HEADERS += tredisdriver.h
SOURCES += tredisdriver.cpp
HEADERS += tredis.h
SOURCES += tredis.cpp
HEADERS += tfileaiologger.h
SOURCES += tfileaiologger.cpp
HEADERS += tfileaiowriter.h
SOURCES += tfileaiowriter.cpp
HEADERS += tscheduler.h
SOURCES += tscheduler.cpp
HEADERS += tapplicationscheduler.h
SOURCES += tapplicationscheduler.cpp
HEADERS += tappsettings.h
SOURCES += tappsettings.cpp
HEADERS += tabstractwebsocket.h
SOURCES += tabstractwebsocket.cpp
HEADERS += twebsocket.h
SOURCES += twebsocket.cpp
HEADERS += twebsocketendpoint.h
SOURCES += twebsocketendpoint.cpp
HEADERS += twebsocketframe.h
SOURCES += twebsocketframe.cpp
HEADERS += twebsocketworker.h
SOURCES += twebsocketworker.cpp
HEADERS += twebsocketsession.h
SOURCES += twebsocketsession.cpp
HEADERS += tpublisher.h
SOURCES += tpublisher.cpp
HEADERS += tsystembus.h
SOURCES += tsystembus.cpp
HEADERS += tprocessinfo.h
SOURCES += tprocessinfo.cpp
HEADERS += tbasictimer.h
SOURCES += tbasictimer.cpp
HEADERS += tatomicptr.h
SOURCES += tatomicptr.cpp
HEADERS += thazardptr.h
SOURCES += thazardptr.cpp
HEADERS += thazardobject.h
SOURCES += thazardobject.cpp
HEADERS += thazardptrmanager.h
SOURCES += thazardptrmanager.cpp
#HEADERS += tsinglylist.h
#SOURCES += tsinglylist.cpp
HEADERS += tatomic.h
SOURCES += tatomic.cpp
HEADERS += tstack.h
SOURCES += tstack.cpp
HEADERS += tqueue.h
SOURCES += tqueue.cpp
HEADERS += tdatabasecontextthread.h
SOURCES += tdatabasecontextthread.cpp
HEADERS += tdatabasecontextmainthread.h
SOURCES += tdatabasecontextmainthread.cpp
HEADERS += tbackgroundprocess.h
SOURCES += tbackgroundprocess.cpp
HEADERS += tbackgroundprocesshandler.h
SOURCES += tbackgroundprocesshandler.cpp
HEADERS += tdebug.h
SOURCES += tdebug.cpp

HEADERS += \
           tfnamespace.h \
           tfcore.h \
           tfexception.h \
           tcookie.h \
           tdispatcher.h \
           tloggerplugin.h \
           tsessionobject.h \
           tsessionstoreplugin.h \
           tjavascriptobject.h \
           tsqlormapper.h \
           tsqljoin.h \
           thttprequestheader.h \
           thttpresponseheader.h \
           tcommandlineinterface.h

windows {
  SOURCES += twebapplication_win.cpp
  SOURCES += tapplicationserverbase_win.cpp
  SOURCES += tfileaiowriter_win.cpp
  SOURCES += tprocessinfo_win.cpp
}
unix {
  HEADERS += tfcore_unix.h
  SOURCES += twebapplication_unix.cpp
  SOURCES += tapplicationserverbase_unix.cpp
  SOURCES += tfileaiowriter_unix.cpp
}
linux-* {
  HEADERS += tmultiplexingserver.h
  SOURCES += tmultiplexingserver_linux.cpp
  HEADERS += tactionworker.h
  SOURCES += tactionworker.cpp
  HEADERS += tepoll.h
  SOURCES += tepoll.cpp
  HEADERS += tepollsocket.h
  SOURCES += tepollsocket.cpp
  HEADERS += tepollhttpsocket.h
  SOURCES += tepollhttpsocket.cpp
  HEADERS += tepollwebsocket.h
  SOURCES += tepollwebsocket.cpp
  SOURCES += tprocessinfo_linux.cpp
}
macx {
  SOURCES += tprocessinfo_macx.cpp
}

# Qt5
greaterThan(QT_MAJOR_VERSION, 4) {
  QT      += qml
  HEADERS += tjsonutil.h
  SOURCES += tjsonutil.cpp
  HEADERS += tjsloader.h
  SOURCES += tjsloader.cpp
  HEADERS += tjsmodule.h
  SOURCES += tjsmodule.cpp
  HEADERS += tjsinstance.h
  SOURCES += tjsinstance.cpp
  HEADERS += treactcomponent.h
  SOURCES += treactcomponent.cpp

  SOURCES += tactioncontroller_qt5.cpp
}


# Files for MongoDB
INCLUDEPATH += ../3rdparty/mongo-c-driver/src/mongoc ../3rdparty/mongo-c-driver/src/libbson/src/bson
windows {
#  DEFINES += MONGO_STATIC_BUILD

  win32-msvc* {
    CONFIG(debug, debug|release) {
      LIBS += ..\3rdparty\mongo-c-driver\debug\mongoc.lib -lws2_32
    } else {
      LIBS += ..\3rdparty\mongo-c-driver\release\mongoc.lib -lws2_32
    }
  } else {
    CONFIG(debug, debug|release) {
      LIBS += ../3rdparty/mongo-c-driver/debug/libmongoc.a -lws2_32
    } else {
      LIBS += ../3rdparty/mongo-c-driver/release/libmongoc.a -lws2_32
    }
  }
} else {
  LIBS += ../3rdparty/mongo-c-driver/libmongoc.a
}
#DEFINES += MONGO_HAVE_STDINT

HEADERS += tmongodriver.h
SOURCES += tmongodriver.cpp
HEADERS += tmongoquery.h
SOURCES += tmongoquery.cpp
HEADERS += tmongocursor.h
SOURCES += tmongocursor.cpp
HEADERS += tbson.h
SOURCES += tbson.cpp
HEADERS += tmongoobject.h
SOURCES += tmongoobject.cpp
HEADERS += tmongoodmapper.h
SOURCES += tmongoodmapper.cpp
HEADERS += tcriteriamongoconverter.h
SOURCES += tcriteriamongoconverter.cpp
