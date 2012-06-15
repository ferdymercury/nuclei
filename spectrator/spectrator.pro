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
    ActiveGraphicsItemGroup.cpp \
    GraphicsHighlightItem.cpp \
    GraphicsDropShadowEffect.cpp \
    ClickableItem.cpp

HEADERS  += Spectrator.h \
    ENSDF.h \
    Decay.h \
    EnergyLevel.h \
    Nuclide.h \
    HalfLife.h \
    SpinParity.h \
    GammaTransition.h \
    ActiveGraphicsItemGroup.h \
    GraphicsHighlightItem.h \
    GraphicsDropShadowEffect.h \
    ClickableItem.h

INCLUDEPATH += ../../libakk/src
LIBS += -lakk -L../../libakk

FORMS    += Spectrator.ui

RESOURCES += \
    spectrator.qrc

# QWT ####################

CONFIG += qwt

exists( /usr/include/qwt/qwt.h ) {
  INCLUDEPATH += /usr/include/qwt
  LIBS += -lqwt
}

exists( /usr/include/qwt6/qwt.h ) {
  INCLUDEPATH += /usr/include/qwt6
  LIBS += -lqwt6
}

exists( /usr/include/qwt-qt4/qwt.h ) {
  INCLUDEPATH += /usr/include/qwt-qt4
  LIBS += -lqwt-qt4
}

exists( C://qwt//include//qwt.h ) {
  INCLUDEPATH += C://qwt//include
  LIBS += -LC://qwt//lib -lqwt6
}
