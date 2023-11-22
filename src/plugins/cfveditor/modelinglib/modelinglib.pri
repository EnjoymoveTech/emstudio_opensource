!isEmpty(QT.svg.name): QT += svg
else: DEFINES += QT_NO_SVG

INCLUDEPATH += $$PWD \
               $$PWD/qtserialization/inc \
               $$PWD/../

include(qstringparser/qstringparser.pri)
include(qtserialization/qtserialization.pri)
include(qmt/qmt.pri)
