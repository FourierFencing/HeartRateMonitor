# Qt project file - qmake uses his to generate a Makefile

QT       += core gui

CONFIG          += qt warn_on debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QwtExample

LIBS += -lqwt -lm -lbcm2835 -lrt -lwiringPi

HEADERS += window.h ADCSetup.h HeartRate.h SPISetup.h

SOURCES += main.cpp window.cpp ADCSetup.cpp HeartRate.cpp SPISetup.cpp
