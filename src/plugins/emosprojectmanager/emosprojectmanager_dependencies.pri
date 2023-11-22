QTC_PLUGIN_NAME = EmosProjectManager
QTC_LIB_DEPENDS += \
    pluginmanager \
    utils
QTC_PLUGIN_DEPENDS += \
    coreplugin \
    explorer \
    generaleditor \
    emoscodebuild\
    emostools

QTC_PLUGIN_RECOMMENDS += \
    #designer

DEFINES *= \
    QMAKE_AS_LIBRARY \
    PROPARSER_THREAD_SAFE \
    PROEVALUATOR_THREAD_SAFE \
    PROEVALUATOR_CUMULATIVE \
    PROEVALUATOR_DUAL_VFS \
    PROEVALUATOR_SETENV
INCLUDEPATH *= $$PWD/../../shared
