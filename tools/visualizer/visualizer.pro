include(../../shared.pri)

TEMPLATE = app
TARGET = visualizer
QT += gui
CONFIG += debug

HEADERS = visualizer.h
SOURCES = visualizer.cpp

target.path = $$PREFIX/bin
INSTALLS = target
