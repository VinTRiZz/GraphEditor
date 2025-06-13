QT += core gui widgets sql

CONFIG += c++17

TARGET = GraphEditor.bin

if (debug | release) {
    TARGET = $$TARGET".Debug"
}

LIBS += -lboost_system \
        -lboost_program_options

include (src/src.pri)
include (build.pri)
