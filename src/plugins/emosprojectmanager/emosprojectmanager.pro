QT += xml
include(../../emos_plugin.pri)

INCLUDEPATH += \
        $$PWD/proparser

DEFINES += QMAKE_LIBRARY
include(proparser/proparser.pri)

DEFINES += \
    EMOSPROJECTMANAGER_LIBRARY

HEADERS += \
    mergewizards/apiwizardpage.h \
    mergewizards/basewizardpage.h \
    mergewizards/cfvwizardpage.h \
    mergewizards/idlwizardpage.h \
    mergewizards/mergewizard.h \
    mergewizards/mergewizardpage.h \
    mergewizards/typewizardpage.h \
    projectmerge.h \
    qmakeparsernodes.h \
    qmakeprojectmanagerplugin.h \
    qmakeproject.h \
    qmakenodes.h \
    qmakenodetreebuilder.h \
    profileeditor.h \
    profilehighlighter.h \
    profilehoverhandler.h \
    qmakeprojectmanagerconstants.h \
    qmakebuildconfiguration.h \
    qmakeparser.h \
    qmakeprojectmanager_global.h \
    profilecompletionassist.h \
    profilereader.h \
    $$PWD/writer/*.h \
    $$PWD/projectimport/*.h

SOURCES += \
    mergewizards/apiwizardpage.cpp \
    mergewizards/basewizardpage.cpp \
    mergewizards/cfvwizardpage.cpp \
    mergewizards/idlwizardpage.cpp \
    mergewizards/mergewizard.cpp \
    mergewizards/mergewizardpage.cpp \
    mergewizards/typewizardpage.cpp \
    projectmerge.cpp \
    qmakeparsernodes.cpp \
    qmakeprojectmanagerplugin.cpp \
    qmakeproject.cpp \
    qmakenodes.cpp \
    qmakenodetreebuilder.cpp \
    profileeditor.cpp \
    profilehighlighter.cpp \
    profilehoverhandler.cpp \
    qmakebuildconfiguration.cpp \
    qmakeparser.cpp \
    profilecompletionassist.cpp \
    profilereader.cpp \
    $$PWD/writer/*.cpp \
    $$PWD/projectimport/*.cpp


RESOURCES += emosprojectmanager.qrc
