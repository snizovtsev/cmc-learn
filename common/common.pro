TEMPLATE = lib
CONFIG += staticlib console

SOURCES += \
    qimagef.cpp \
    util.cpp \
    hog.cpp \
    idlparser.cpp \
    tweaks.cpp \
    detection.cpp

HEADERS += \
    qimagef.h \
    util.h \
    hog.h \
    idlparser.h \
    tweaks.h \
    detection.h

LIBS += -llinear
