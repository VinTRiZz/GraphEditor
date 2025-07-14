#include "testgraphgenerator.h"

#include "generators.h"
#include "logging.h"

#include <QIcon>

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

    result.setName("Test example graph");
    result.setDescription("Example description");
    result.setCreateTime(QDateTime::currentDateTime());
    result.setEditTime(QDateTime::currentDateTime());

//    for (uint i = 0; i < vertexCount; ++i) {
//        result.addVertex(generateVertex(i));
//    }

//    if (vertexCount > 0) {
//        std::list<uint> addedVertexes;
//        auto vertices = result.getAllVertices();
//        for (uint i = 0; i < vertexCount; ++i) {
//            for (uint j = 0; j < 3; ++j) {
//                auto targetVertex = vertices.at(std::abs(std::rand()) % vertices.size());
//                while (targetVertex.id == i) {
//                    targetVertex = vertices.at(std::abs(std::rand()) % vertices.size());
//                }
//                result.addConnection(generateConnection(i, targetVertex.id));
//            }
//        }
//    }

    // TODO: Remove hardcode
    Graph::GVertex vert;
    vert.shortName = "Дебич";
    vert.name = "Дебич узел";
    vert.backgroundColor = Qt::green;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 100;
    vert.posY = 100;
    result.addVertex(vert);

    vert.shortName = "Кр. дебич";
    vert.name = "Кр. дебич узел";
    vert.backgroundColor = Qt::red;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 300;
    vert.posY = 300;
    result.addVertex(vert);

    vert.shortName = "Др. дебич";
    vert.name = "Др. дебич узел";
    vert.backgroundColor = Qt::green;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_green.png").pixmap(500, 500);
    vert.posX = 500;
    vert.posY = 500;
    result.addVertex(vert);

    vert.shortName = "Кр. хер";
    vert.name = "Кр. хер узел";
    vert.backgroundColor = Qt::red;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 100;
    vert.posY = 500;
    result.addVertex(vert);

    Graph::GConnection con;
    con.name = "1-2";
    con.idFrom = 1;
    con.idTo = 2;
    con.lineColor = Qt::red;
    result.addConnection(con);

    con.name = "4-3";
    con.idFrom = 4;
    con.idTo = 3;
    con.lineColor = Qt::green;
    result.addConnection(con);

    con.name = "1-3";
    con.idFrom = 1;
    con.idTo = 3;
    con.lineColor = Qt::magenta;
    result.addConnection(con);

    return result;
}
