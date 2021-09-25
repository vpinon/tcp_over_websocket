TARGET = tcp2web
TEMPLATE = app
SOURCES += tcp2web.cpp
HEADERS += tcp2web.h
QT       += core websockets network
QT       -= gui
CONFIG   += console
CONFIG   -= app_bundle
