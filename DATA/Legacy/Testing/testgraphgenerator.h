#ifndef TESTGRAPHGENERATOR_H
#define TESTGRAPHGENERATOR_H

#include <boost/noncopyable.hpp>

#include "Graph/graphobject.h"

class TestGraphGenerator : public boost::noncopyable
{
public:
    static Graph::GConnection  generateConnection(GraphCommon::graphId_t idFrom, GraphCommon::graphId_t idTo);
    static Graph::GVertex      generateVertex(GraphCommon::graphId_t id);
    static Graph::GraphObject  generateGraph(GraphCommon::graphId_t vertexCount);
};

#endif // TESTGRAPHGENERATOR_H
