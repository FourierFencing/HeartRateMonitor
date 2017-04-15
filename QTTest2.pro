# Qt project file - qmake uses his to generate a Makefile

QT       += core gui

CONFIG          += qt warn_on debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QwtTest

LIBS += -lqwt -lm -lbcm2835 -lrt -lwiringPi -lfftw3 -liir

HEADERS += window.h ADCSetup.h adcReader2.h SPISetup.h

SOURCES += main.cpp window.cpp ADCSetup.cpp adcReader2.cpp SPISetup.cpp

