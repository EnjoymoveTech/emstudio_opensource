DEFINES += CORE_LIBRARY
QT += \
    network \
    printsupport \
    qml \
    sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../emos_plugin.pri)
msvc: QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += \
        $$PWD/*.cpp\
        $$PWD/editormanager/*.cpp\
        $$PWD/dialogs/*.cpp\
        $$PWD/actionmanager/*.cpp\
        progressmanager/progressmanager.cpp \
        progressmanager/progressview.cpp \
        progressmanager/progressbar.cpp \
        progressmanager/futureprogress.cpp \

HEADERS += \
        $$PWD/*.h\
        $$PWD/editormanager/*.h\
        $$PWD/dialogs/*.h\
        $$PWD/actionmanager/*.h\
        progressmanager/progressmanager_p.h \
        progressmanager/progressview.h \
        progressmanager/progressbar.h \
        progressmanager/futureprogress.h \
        progressmanager/progressmanager.h \

FORMS += dialogs/newdialog.ui \
    dialogs/saveitemsdialog.ui \
    dialogs/readonlyfilesdialog.ui \
    dialogs/openwithdialog.ui \
    generalsettings.ui \
    dialogs/externaltoolconfig.ui \
    dialogs/filepropertiesdialog.ui \
    mimetypesettingspage.ui \
    mimetypemagicdialog.ui \
    dialogs/addtovcsdialog.ui \
    systemsettings.ui

RESOURCES += core.qrc \
    fancyactionbar.qrc

include(find/find.pri)
include(locator/locator.pri)

win32 {
    SOURCES += progressmanager/progressmanager_win.cpp
    QT += gui-private # Uses QPlatformNativeInterface.
    LIBS += -lole32 -luser32
}
else:macx {
    OBJECTIVE_SOURCES += \
        progressmanager/progressmanager_mac.mm
    LIBS += -framework AppKit
}
else:unix {
    SOURCES += progressmanager/progressmanager_x11.cpp

    IMAGE_SIZE_LIST = 16 24 32 48 64 128 256 512

    for(imagesize, IMAGE_SIZE_LIST) {
        eval(image$${imagesize}.files = images/logo/$${imagesize}/EmStudio-emstudio.png)
        eval(image$${imagesize}.path = $$QTC_PREFIX/share/icons/hicolor/$${imagesize}x$${imagesize}/apps)
        INSTALLS += image$${imagesize}
    }
}
