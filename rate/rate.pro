TARGET = ../bin/rate
INCLUDEPATH += ../common
LIBS += -L../common -lcommon
SOURCES += rate.cpp
PRE_TARGETDEPS = ../common/libcommon.a
