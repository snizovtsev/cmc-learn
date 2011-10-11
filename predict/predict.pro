TARGET = ../bin/predict
INCLUDEPATH += ../common
LIBS += -L../common -lcommon -llinear
SOURCES += predict.cpp
PRE_TARGETDEPS = ../common/libcommon.a
