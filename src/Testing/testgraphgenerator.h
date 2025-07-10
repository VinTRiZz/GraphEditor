#ifndef TESTGRAPHGENERATOR_H
#define TESTGRAPHGENERATOR_H

#include <boost/noncopyable.hpp>

#include "Graph/graphobject.h"

class TestGraphGenerator : public boost::noncopyable
{
public:
    static Graph::GConnection  generateConnection(uint idFrom, uint idTo);
    static Graph::GVertex      generateVertex(uint id);
    static Graph::GraphObject  generateGraph(uint vertexCount);
};

#endif // TESTGRAPHGENERATOR_H
