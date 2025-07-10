#include "testgraphgenerator.h"

#include "generators.h"

#include "logging.h"

Graph::GConnection TestGraphGenerator::generateConnection(uint idFrom, uint idTo)
{
    Graph::GConnection res;

    res.connectionWeight = std::abs(std::rand()) % 1000 + 1.0 / static_cast<double>(std::abs(std::rand()) % 1000);
    res.idFrom = idFrom;
    res.idTo = idTo;

    res.isDirected = true;
    res.lineColor = Qt::black;

    LOG_DEBUG_SYNC("Created connection:", idFrom, "->", idTo);
    return res;
}

Graph::GVertex TestGraphGenerator::generateVertex(uint id)
{
    Graph::GVertex result;
    result.id = id;
    result.name = Utility::Generators::generateStringQ(3, 30, true);
    result.shortName = Utility::Generators::generateStringQ(2, 10, true);
    result.description = Utility::Generators::generateStringQ(40, 70, true);
    result.backgroundColor = Qt::white;
    result.borderColor = Qt::black;

    result.posY = (id + 1) * 100 + std::copysign(100, std::rand());
    result.posX = (id + 1) * 200 + std::copysign(100, std::rand());

    LOG_DEBUG_SYNC("Created vertex:", id, "pos:", result.posX, result.posY);
    return result;
}

Graph::GraphObject TestGraphGenerator::generateGraph(uint vertexCount)
{
    Graph::GraphObject result;
    for (uint i = 0; i < vertexCount; ++i) {
        result.addVertex(generateVertex(i));
    }

    if (vertexCount > 0) {
        std::list<uint> addedVertexes;
        auto vertices = result.getAllVertices();
        for (uint i = 0; i < vertexCount; ++i) {
            for (uint j = 0; j < 3; ++j) {
                auto targetVertex = vertices.at(std::abs(std::rand()) % vertices.size());
                while (targetVertex.id == i) {
                    targetVertex = vertices.at(std::abs(std::rand()) % vertices.size());
                }
                result.addConnection(generateConnection(i, targetVertex.id));
            }
        }
    }

    return result;
}
