include(../../../emos_lib.pri)
include(autogenerated/autogenerated.pri)

QT += network

DEFINES += KSYNTAXHIGHLIGHTING_LIBRARY

RESOURCES += \
    data/themes/theme-data.qrc

HEADERS += \
    src/lib/abstracthighlighter.h \
    src/lib/abstracthighlighter_p.h \
    src/lib/context_p.h \
    src/lib/contextswitch_p.h \
    src/lib/definition.h \
    src/lib/definition_p.h \
    src/lib/definitiondownloader.h \
    src/lib/definitionref_p.h \
    src/lib/foldingregion.h \
    src/lib/format.h \
    src/lib/format_p.h \
    src/lib/htmlhighlighter.h \
    src/lib/keywordlist_p.h \
    src/lib/ksyntaxhighlighting_export.h \
    src/lib/matchresult_p.h \
    src/lib/repository.h \
    src/lib/repository_p.h \
    src/lib/rule_p.h \
    src/lib/state.h \
    src/lib/state_p.h \
    src/lib/syntaxhighlighter.h \
    src/lib/textstyledata_p.h \
    src/lib/theme.h \
    src/lib/themedata_p.h \
    src/lib/wildcardmatcher_p.h \
    src/lib/worddelimiters_p.h \
    src/lib/xml_p.h \

SOURCES += \
    src/lib/abstracthighlighter.cpp \
    src/lib/context.cpp \
    src/lib/contextswitch.cpp \
    src/lib/definition.cpp \
    src/lib/definitiondownloader.cpp \
    src/lib/foldingregion.cpp \
    src/lib/format.cpp \
    src/lib/htmlhighlighter.cpp \
    src/lib/keywordlist.cpp \
    src/lib/repository.cpp \
    src/lib/rule.cpp \
    src/lib/state.cpp \
    src/lib/syntaxhighlighter.cpp \
    src/lib/theme.cpp \
    src/lib/themedata.cpp \
    src/lib/wildcardmatcher.cpp \
    src/lib/worddelimiters.cpp \
