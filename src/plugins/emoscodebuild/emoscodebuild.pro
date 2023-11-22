DEFINES += EMOSCODEBUILD_LIBRARY
#isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "D:/EnjoyMove/Code/CodeRoot_1210_qm/src/emstudio_fqm1111"
#isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "D:/EnjoyMove/Code/CodeRoot_1210_qm/src/build-emostudio-Desktop_Qt_5_12_12_MSVC2017_64bit-Debug111"


isEmpty(IDE_SOURCE_TREE): include(../../emos_plugin.pri)

QT += xml
QT += core
# EmosCodeBuild files

DEFINES += EMOSCODEBUILD_LIBRARY

INCLUDEPATH += .\
           $$PWD \
           $$PWD/inja \
           $$PWD/configbuild \
           $$PWD/someip \
           $$PWD/buildoutpane \
           $$IDE_BUILD_TREE/src/libs/thirdparty

SOURCES += \
           $$PWD/*.cpp \
           $$PWD/inja/*.cpp \
           $$PWD/configbuild/*.cpp \
           $$PWD/someip/*.cpp \
           $$PWD/buildoutpane/*.cpp

HEADERS += \
           $$PWD/*.h \
           $$PWD/inja/*.h \
           $$PWD/configbuild/*.h \
           $$PWD/someip/*.h \
           $$PWD/buildoutpane/*.h

FORMS += \
    $$PWD/*.ui \
    $$PWD/inja/*.ui


