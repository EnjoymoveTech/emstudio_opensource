# USE .subdir AND .depends !cqcq
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

win32: LIBS *= -ladvapi32 -lUser32

include (../../emos_lib.pri)

QT += widgets gui network qml xml
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat


CONFIG += exceptions # used by portlist.cpp, textfileformat.cpp, and ssh/*

shared {
    DEFINES += UTILS_LIBRARY
} else {
    DEFINES += EMSTUDIO_UTILS_STATIC_LIB
}

!win32:{
    isEmpty(IDE_LIBEXEC_PATH) | isEmpty(IDE_BIN_PATH): {
        warning("using utils-lib.pri without IDE_LIBEXEC_PATH or IDE_BIN_PATH results in empty QTC_REL_TOOLS_PATH")
        DEFINES += QTC_REL_TOOLS_PATH=$$shell_quote(\"\")
    } else {
        DEFINES += QTC_REL_TOOLS_PATH=$$shell_quote(\"$$relative_path($$IDE_LIBEXEC_PATH, $$IDE_BIN_PATH)\")
    }
}

SOURCES += \
          $$PWD/*.cpp\
          $$PWD/theme/*.cpp\
          $$PWD/tooltip/*.cpp\
          $$PWD/control/*.cpp


HEADERS += \
            $$PWD/*.h \
            $$PWD/theme/*.h\
            $$PWD/tooltip/*.h\
            $$PWD/control/*.h\

FORMS += $$PWD/filewizardpage.ui \
    $$PWD/projectintropage.ui \
    $$PWD/proxycredentialsdialog.ui \
    $$PWD/removefiledialog.ui

RESOURCES += $$PWD/utils.qrc

include(touchbar/touchbar.pri)
include(mimetypes/mimetypes.pri)
include(emoswidgets/emoswidgets.pri)
