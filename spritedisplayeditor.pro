TEMPLATE = app
TARGET = tool

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main_window.cpp \
           dialogs.cpp \
           tool.cpp \
           sprite.cpp \
           file_formats.cpp \
           sprite_tools.cpp \
           map16.cpp \
           ext/libsmw.cpp \
           ext/crc32.cpp \
           main.cpp

HEADERS += main_window.h \
           dialogs.h \
           tool.h \
           sprite.h \
           file_formats.h \
           sprite_tools.h \
           ext/libsmw.h \
           ext/crc32.h \
           ext/asar_errors_small.h

CONFIG += debug_and_release

Release:DESTDIR = release
Release:OBJECTS_DIR = release/obj

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj

#win32:RC_ICONS = stuff/icon.ico
