TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    qimagef.cpp \
    util.cpp \
    hog.cpp

HEADERS += \
    qimagef.h \
    util.h \
    hog.h

LIBS += -llinear
