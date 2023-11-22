
include(../../emos_plugin.pri)
QT += xml core
# EmosCodeBuild files

DEFINES += EMOSTOOLS_LIBRARY

INCLUDEPATH += \
           $$PWD \
           $$IDE_BUILD_TREE/src/libs/thirdparty

SOURCES += $$PWD/*.cpp
HEADERS += $$PWD/*.h
