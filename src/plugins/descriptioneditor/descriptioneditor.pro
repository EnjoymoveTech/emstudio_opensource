DEFINES += EMOSCODEBUILD_LIBRARY
#isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = "D:/EnjoyMove/Code/CodeRoot_1210_qm/src/emstudio_fqm1111"
#isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = "D:/EnjoyMove/Code/CodeRoot_1210_qm/src/build-emostudio-Desktop_Qt_5_12_12_MSVC2017_64bit-Debug111"

isEmpty(IDE_SOURCE_TREE): include(../../emos_plugin.pri)
QT += xml
QT += core
# EmosCodeBuild files

DEFINES += DESCRIPTIONEDITOR_LIBRARY

SOURCES += $$PWD/*.cpp \
           $$PWD/apiwidget/*.cpp \
           $$PWD/typewidget/*.cpp \
           $$PWD/typewidget/fileparser/*.cpp

HEADERS += $$PWD/*.h \
           $$PWD/apiwidget/*.h \
           $$PWD/typewidget/*.h \
           $$PWD/typewidget/fileparser/*.h

FORMS += \

INCLUDEPATH += ./ \
        $$PWD/apiwidget \
        $$PWD/typewidget

RESOURCES += \
