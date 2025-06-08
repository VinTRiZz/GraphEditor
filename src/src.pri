SOURCES +=  $$PWD/*.cpp
HEADERS +=  $$PWD/*.h

INCLUDEPATH += $$PWD

include(GUI/GUI.pri)
include(Graph/Graph.pri)
include(Internal/Internal.pri)
