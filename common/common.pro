TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    qimagef.cpp \
    util.cpp \
    hog.cpp \
    idlparser.cpp

HEADERS += \
    qimagef.h \
    util.h \
    hog.h \
    idlparser.h

LIBS += -llinear
