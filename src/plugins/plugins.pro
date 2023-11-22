include(../../emos_x.pri)

TEMPLATE  = subdirs

CONFIG += ordered

SUBDIRS   = \
    coreplugin\
    generaleditor\
    explorer\
    helpme\
    emostools\
    emoscodebuild\
    emosprojectmanager\
    descriptioneditor\
    idleditor\
    cfveditor

isEmpty(QBS_INSTALL_DIR): QBS_INSTALL_DIR = $$(QBS_INSTALL_DIR)



QTC_ENABLE_CLANG_REFACTORING=$$(QTC_ENABLE_CLANG_REFACTORING)



#for(p, SUBDIRS) {
#    QTC_PLUGIN_DEPENDS =
#    include($$p/$${p}_dependencies.pri)
#    pv = $${p}.depends
#    $$pv = $$QTC_PLUGIN_DEPENDS
#}

linux-* {
     #SUBDIRS += debugger/ptracepreload.pro
}

QMAKE_EXTRA_TARGETS += deployqt # dummy
