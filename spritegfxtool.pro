TEMPLATE = app
TARGET = tool

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += window.cpp sprite.cpp main.cpp
HEADERS += window.h sprite.h
CONFIG += debug_and_release

Release:DESTDIR = release
Release:OBJECTS_DIR = release/obj

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj

#win32:RC_ICONS = stuff/icon.ico
