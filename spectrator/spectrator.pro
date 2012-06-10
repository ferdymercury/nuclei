#-------------------------------------------------
#
# Project created by QtCreator 2012-05-29T18:21:38
#
#-------------------------------------------------

QT       += core gui

TARGET = spectrator
TEMPLATE = app


SOURCES += main.cpp\
        Spectrator.cpp \
    ENSDF.cpp \
    Decay.cpp \
    EnergyLevel.cpp \
    Nuclide.cpp \
    HalfLife.cpp \
    SpinParity.cpp \
    GammaTransition.cpp \
    ActiveGraphicsItemGroup.cpp

HEADERS  += Spectrator.h \
    ENSDF.h \
    Decay.h \
    EnergyLevel.h \
    Nuclide.h \
    HalfLife.h \
    SpinParity.h \
    GammaTransition.h \
    ActiveGraphicsItemGroup.h

FORMS    += Spectrator.ui
