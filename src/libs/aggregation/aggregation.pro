include (../../emos_lib.pri)

DEFINES += AGGREGATION_LIBRARY

shared {
    DEFINES += UTILS_LIBRARY
} else {
    DEFINES += EMSTUDIO_UTILS_STATIC_LIB
}


HEADERS +=\
    aggregate.h \
    aggregation_global.h

SOURCES += \
       aggregate.cpp

