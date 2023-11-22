include(../../emos_plugin.pri)
QT += network gui-private

SOURCES += helpmeplugin.cpp \
    docwidget.cpp \
    postmessager.cpp \
    bugreportwidget.cpp \
    welcomewidget.cpp

DEFINES += HELPME_LIBRARY

HEADERS += \
    docwidget.h \
    postmessager.h \
    bugreportwidget.h \
    welcomewidget.h

FORMS += \
    bugreportwidget.ui
