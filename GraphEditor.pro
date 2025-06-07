QT       += core gui widgets

CONFIG += c++17

TARGET = GraphEditor.bin

if (debug | release) {
    TARGET = $$TARGET".Debug"
}

include (src/src.pri)
include (build.pri)
