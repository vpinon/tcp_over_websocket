TARGET = web2tcp
TEMPLATE = app
SOURCES += web2tcp.cpp
HEADERS += web2tcp.h
QT       += core websockets network
QT       -= gui
CONFIG   += console
CONFIG   -= app_bundle
