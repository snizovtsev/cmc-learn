TARGET = ../bin/learn
INCLUDEPATH += ../common
LIBS += -L../common -lcommon -llinear
SOURCES += learn.cpp
PRE_TARGETDEPS = ../common/libcommon.a
