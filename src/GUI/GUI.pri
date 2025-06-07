SOURCES +=  $$PWD/*.cpp
HEADERS +=  $$PWD/*.h
FORMS   +=  $$PWD/*.ui

INCLUDEPATH += $$PWD
include (ObjectScene/ObjectScene.pri)
include (Graph/Graph.pri)
