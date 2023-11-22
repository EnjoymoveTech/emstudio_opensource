QT += qml

include(../../emos_plugin.pri)
include(customwizard/customwizard.pri)
include(jsonwizard/jsonwizard.pri)

#include(../../shared/clang/clang_installation.pri)
#include(../../shared/clang/clang_defines.pri)

HEADERS += explorer.h \
    abi.h \
    buildaspects.h \
    buildinfo.h \
    buildpropertiessettings.h \
    buildsystem.h \
    buildtargettype.h \
    coderootselector.h \
    desktoprunconfiguration.h \
    environmentaspect.h \
    environmentaspectwidget.h \
    fileinsessionfinder.h \
    filterkitaspectsdialog.h \
    importwidget.h \
    userfileaccessor.h \
    localenvironmentaspect.h \
    osparser.h \
    explorer_export.h \
    projectimporter.h \
    projectwindow.h \
    kit.h \
    kitchooser.h \
    kitinformation.h \
    kitfeatureprovider.h \
    kitmanager.h \
    projectconfigurationmodel.h \
    buildmanager.h \
    buildsteplist.h \
    deployconfiguration.h \
    namedwidget.h \
    target.h \
    task.h \
    taskmodel.h \
    projectfilewizardextension.h \
    session.h \
    allprojectsfilter.h \
    ioutputparser.h \
    projectconfiguration.h \
    projectexplorerconstants.h \
    projectexplorersettings.h \
    project.h \
    projectmanager.h \
    currentprojectfilter.h \
    allprojectsfind.h \
    buildstep.h \
    buildconfiguration.h \
    environmentwidget.h \
    processstep.h \
    editorconfiguration.h \
    runconfiguration.h \
    runcontrol.h \
    applicationlauncher.h \
    projecttreewidget.h \
    foldernavigationwidget.h \
    projectnodes.h \
    sessiondialog.h \
    sessionview.h \
    projectwizardpage.h \
    buildstepspage.h \
    projectmodels.h \
    currentprojectfind.h \
    toolchain.h \
    toolchaincache.h \
    toolchainconfigwidget.h \
    toolchainmanager.h \
    toolchainsettingsaccessor.h \
    baseprojectwizarddialog.h \
    runconfigurationaspects.h \
    processparameters.h \
    abstractprocessstep.h \
    taskhub.h \
    headerpath.h \
    appoutputpane.h \
    deployablefile.h \
    devicesupport/idevice.h \
    devicesupport/desktopdevice.h \
    devicesupport/desktopdevicefactory.h \
    devicesupport/idevicewidget.h \
    devicesupport/idevicefactory.h \
    devicesupport/desktopdeviceprocess.h \
    devicesupport/devicecheckbuildstep.h \
    devicesupport/devicemanager.h \
    devicesupport/devicemanagermodel.h \
    devicesupport/devicefactoryselectiondialog.h \
    devicesupport/deviceprocess.h \
    devicesupport/deviceprocesslist.h \
    devicesupport/deviceprocessesdialog.h \
    devicesupport/devicesettingswidget.h \
    devicesupport/devicesettingspage.h \
    devicesupport/devicetestdialog.h \
    devicesupport/deviceusedportsgatherer.h \
    devicesupport/localprocesslist.h \
    devicesupport/sshdeviceprocess.h \
    devicesupport/sshdeviceprocesslist.h \
    devicesupport/sshsettingspage.h \
    devicesupport/desktopprocesssignaloperation.h \
    deploymentdata.h \
    deploymentdataview.h \
    buildtargetinfo.h \
    projectmacroexpander.h \
    ipotentialkit.h \
    selectablefilesmodel.h \
    panelswidget.h \
    projectwelcomepage.h \
    sessionmodel.h \
    projectpanelfactory.h \
    projecttree.h \
    expanddata.h \
    waitforstopdialog.h \
    projectexplorericons.h \
    projectmacro.h \
    makestep.h \
    parseissuesdialog.h \
    treescanner.h

SOURCES += explorer.cpp \
    abi.cpp \
    buildaspects.cpp \
    buildinfo.cpp \
    buildsystem.cpp \
    coderootselector.cpp \
    desktoprunconfiguration.cpp \
    environmentaspect.cpp \
    environmentaspectwidget.cpp \
    fileinsessionfinder.cpp \
    filterkitaspectsdialog.cpp \
    importwidget.cpp \
    projectconfigurationmodel.cpp \
    userfileaccessor.cpp \
    localenvironmentaspect.cpp \
    osparser.cpp \
    projectimporter.cpp \
    projectwindow.cpp \
    kit.cpp \
    kitchooser.cpp \
    kitinformation.cpp \
    kitmanager.cpp \
    buildmanager.cpp \
    buildsteplist.cpp \
    deployconfiguration.cpp \
    namedwidget.cpp \
    target.cpp \
    ioutputparser.cpp \
    projectconfiguration.cpp \
    task.cpp \
    taskmodel.cpp \
    projectfilewizardextension.cpp \
    session.cpp \
    allprojectsfilter.cpp \
    currentprojectfilter.cpp \
    allprojectsfind.cpp \
    project.cpp \
    buildstep.cpp \
    buildconfiguration.cpp \
    environmentwidget.cpp \
    processstep.cpp \
    abstractprocessstep.cpp \
    editorconfiguration.cpp \
    runconfiguration.cpp \
    runcontrol.cpp \
    applicationlauncher.cpp \
    projecttreewidget.cpp \
    foldernavigationwidget.cpp \
    projectnodes.cpp \
    sessiondialog.cpp \
    sessionview.cpp \
    projectwizardpage.cpp \
    buildstepspage.cpp \
    projectmodels.cpp \
    currentprojectfind.cpp \
    toolchain.cpp \
    toolchainconfigwidget.cpp \
    toolchainmanager.cpp \
    toolchainsettingsaccessor.cpp \
    baseprojectwizarddialog.cpp \
    runconfigurationaspects.cpp \
    taskhub.cpp \
    processparameters.cpp \
    appoutputpane.cpp \
    devicesupport/idevice.cpp \
    devicesupport/desktopdevice.cpp \
    devicesupport/desktopdevicefactory.cpp \
    devicesupport/idevicefactory.cpp \
    devicesupport/desktopdeviceprocess.cpp \
    devicesupport/devicecheckbuildstep.cpp \
    devicesupport/devicemanager.cpp \
    devicesupport/devicemanagermodel.cpp \
    devicesupport/devicefactoryselectiondialog.cpp \
    devicesupport/deviceprocess.cpp \
    devicesupport/deviceprocesslist.cpp \
    devicesupport/deviceprocessesdialog.cpp \
    devicesupport/devicesettingswidget.cpp \
    devicesupport/devicesettingspage.cpp \
    devicesupport/devicetestdialog.cpp \
    devicesupport/deviceusedportsgatherer.cpp \
    devicesupport/localprocesslist.cpp \
    devicesupport/sshdeviceprocess.cpp \
    devicesupport/sshdeviceprocesslist.cpp \
    devicesupport/sshsettingspage.cpp \
    devicesupport/desktopprocesssignaloperation.cpp \
    deployablefile.cpp \
    deploymentdata.cpp \
    deploymentdataview.cpp \
    projectmacroexpander.cpp \
    selectablefilesmodel.cpp \
    panelswidget.cpp \
    projectwelcomepage.cpp \
    sessionmodel.cpp \
    projectpanelfactory.cpp \
    projecttree.cpp \
    expanddata.cpp \
    waitforstopdialog.cpp \
    projectexplorericons.cpp \
    projectmacro.cpp \
    makestep.cpp \
    parseissuesdialog.cpp \
    treescanner.cpp

FORMS += \
    sessiondialog.ui \
    projectwizardpage.ui \
    devicesupport/devicefactoryselectiondialog.ui \
    devicesupport/devicesettingswidget.ui \
    devicesupport/devicetestdialog.ui

equals(TEST, 1) {
    SOURCES += \
        outputparser_test.cpp
    HEADERS += \
        outputparser_test.h
}

journald {
    SOURCES += journaldwatcher.cpp
    HEADERS += journaldwatcher.h
    DEFINES += WITH_JOURNALD
    LIBS += -lsystemd
}

RESOURCES += explorer.qrc

DEFINES += PROJECTEXPLORER_LIBRARY

!isEmpty(PROJECT_USER_FILE_EXTENSION) {
    DEFINES += PROJECT_USER_FILE_EXTENSION=$${PROJECT_USER_FILE_EXTENSION}
}
