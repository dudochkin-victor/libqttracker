include(../../shared.pri)

TEMPLATE = app
TARGET = tracker-browser
LIBS = -lqttracker
QT += gui

HEADERS = browser.h
SOURCES = browser.cpp

target.path = $$PREFIX/bin
INSTALLS = target
