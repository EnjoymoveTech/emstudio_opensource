include(../../emos_x.pri)

TEMPLATE  = subdirs

SUBDIRS   += \
    aggregation \
    pluginmanager \
    utils \
    ssh \
    xlsx

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}

include(../shared/syntax/syntax_shared.pri)
isEmpty(KSYNTAXHIGHLIGHTING_LIB_DIR) {
    SUBDIRS += \
        thirdparty/syntax-highlighting \
        thirdparty/syntax-highlighting/data

    equals(KSYNTAX_WARN_ON, 1) {
        message("Either KSYNTAXHIGHLIGHTING_LIB_DIR does not exist or include path could not be deduced.")
        unset(KSYNTAX_WARN_ON)
    }
} else {
    message("Using KSyntaxHighlighting provided at $${KSYNTAXHIGHLIGHTING_LIB_DIR}.")
}

QMAKE_EXTRA_TARGETS += deployqt # dummy
