TEMPLATE = app
TARGET = 
DEPENDPATH += .

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker

# Input
HEADERS += classupdatesignaler.h
SOURCES += classupdatesignaler.cpp
