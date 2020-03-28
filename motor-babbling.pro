TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lYARP_OS -lYARP_sig -lYARP_init -liKin -lYARP_dev -lYARP_math

INCLUDEPATH += /home/gabriele/usr/include

LIBS += -L/home/gabriele/usr/lib -lutilities -lsec -lrobots

SOURCES += main.cpp \
    babblinggenerator.cpp \
    icubgazeborunner.cpp \
    movewristvel.cpp \
    forward_kin.cpp \
    safetystopper.cpp \
    wristoffset.cpp \
    torqueestimations.cpp

HEADERS += \
    babblinggenerator.h \
    icubgazeborunner.h \
    movewristvel.h \
    forward_kin.h \
    safetystopper.h \
    wristoffset.h \
    torqueestimations.h
