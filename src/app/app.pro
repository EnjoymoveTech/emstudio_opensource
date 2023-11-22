# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)
include (../../emos_x.pri)
include(../shared/qtsingleapplication/qtsingleapplication.pri)

QT += core gui

macx {
    QT += xml
}

DESTDIR = $$IDE_APP_PATH
TEMPLATE = app
TARGET = $$IDE_APP_TARGET
DESTDIR = $$IDE_APP_PATH

LIBS += -L$$IDE_LIBRARY_PATH

INCLUDEPATH += $$IDE_SOURCE_TREE/src/api
INCLUDEPATH += $$IDE_SOURCE_TREE/src/utils
INCLUDEPATH += $$IDE_SOURCE_TREE/src/thirdparty

SOURCES += main.cpp

HEADERS  +=

RPATH_BASE = $$IDE_BIN_PATH
include(../rpath.pri)

#CONFIG += console
LIBS *= -l$$qtLibraryName(PluginManager) -l$$qtLibraryName(Aggregation) -l$$qtLibraryName(Utils)

win32{
    COPYRIGHT = "2008-$${EMSTUDIO_COPYRIGHT_YEAR} The Qt Company Ltd"
    APPLICATION_NAME = "$${IDE_DISPLAY_NAME}"
    DEFINES += \
        RC_APPLICATION_NAME=\"$$replace(APPLICATION_NAME, " ", "\\x20")\" \
        RC_VERSION=$$replace(EMSTUDIO_VERSION, "\\.", ","),0 \
        RC_VERSION_STRING=\"$${EMSTUDIO_DISPLAY_VERSION}\" \
        RC_COPYRIGHT=\"$$replace(COPYRIGHT, " ", "\\x20")\"
    RC_FILE = emos.rc
}else:linux-* {
    QMAKE_LFLAGS += -no-pie
}


target.path = /bin
INSTALLS += target
