VPATH += $$PWD
QT += xml

INCLUDEPATH *= $$PWD $$PWD/..
DEPENDPATH *= $$PWD $$PWD/..

# Input
HEADERS += \
        $$PWD/*.h

SOURCES += \
        $$PWD/*.cpp

RESOURCES += $$PWD/*.qrc
DEFINES += QMAKE_BUILTIN_PRFS QMAKE_OVERRIDE_PRFS
win32: LIBS *= -ladvapi32
