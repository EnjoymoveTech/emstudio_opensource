DEFINES += IDL_LIBRARY

include(../../emos_plugin.pri)
include(./subwindows/subwindows.pri)

QT += xml
QT += core

INCLUDEPATH += ./ \
               ./subwindows

HEADERS += $$PWD/*.h

SOURCES +=$$PWD/*.cpp

RESOURCES += \
    picture.qrc
