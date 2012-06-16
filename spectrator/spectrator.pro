QT       += core gui network

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
    ClickableItem.cpp \
    qxtgroupbox.cpp \
    ENSDFDownloader.cpp

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
    ClickableItem.h \
    version.h \
    qxtgroupbox.h \
    ENSDFDownloader.h

INCLUDEPATH += ../../libakk/src
LIBS += -lakk -L../../libakk
LIBS += -lquazip

FORMS    += Spectrator.ui \
    ENSDFDownloader.ui \
    ENSDFDownloaderSettings.ui

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

exists( C://build//qwt//src//qwt.h ) {
  INCLUDEPATH += C://build//qwt//src
  LIBS += -LC://build//qwt//lib -lqwt
}

RC_FILE = spectrator.rc

