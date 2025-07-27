#ifndef GRAPHMAINTANER_H
#define GRAPHMAINTANER_H

#include "graphobject.h"
#include "graphextendedobject.h"

namespace Graph
{

class GraphMaintaner
{
public:
    GraphMaintaner();
    ~GraphMaintaner();

    static std::shared_ptr<GraphMaintaner> createInstance();

    GraphObject* getObject();
    GraphExtendedObject* getExtendedObject();

private:
    GraphExtendedObject m_graph;
};

}

#endif // GRAPHMAINTANER_H
