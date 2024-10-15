INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/AnimeGO/include
INCLUDEPATH += $$PWD/Kodik/include
LIBS += -lcurl -lxml2
SOURCES += src/main.cpp
SOURCES += src/MainWindow.cpp
SOURCES += AnimeGO/src/Assistive.cpp
SOURCES += AnimeGO/src/cJSON.c
SOURCES += AnimeGO/src/AnimeGO.cpp
SOURCES += src/TitleButton.cpp
SOURCES += src/EpisodeButton.cpp
SOURCES += src/DubButton.cpp
SOURCES += Kodik/src/Kodik.cpp
SOURCES += Kodik/src/Base64.cpp
HEADERS += include/TitleButton.h
HEADERS += include/DubButton.h
HEADERS += include/MainWindow.h
HEADERS += include/EpisodeButton.h
TARGET = AnimeWatch
QT += widgets

CONFIG += debug
QMAKE_CXXFLAGS += -O0 -g -ggdb

