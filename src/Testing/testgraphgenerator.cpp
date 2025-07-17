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

    auto redPersonIcon = QIcon("://DATA/images/vertexicons/person/red.svg");
    auto redPersonImage = redPersonIcon.pixmap(500).toImage();

    auto greenPersonIcon = QIcon("://DATA/images/vertexicons/person/green.svg");
    auto greenPersonImage = greenPersonIcon.pixmap(500).toImage();

    Graph::GVertex vert;
    vert.shortName = "Нода 1";
    vert.name = "Соединён с 2 и 3";
    vert.backgroundColor = Qt::green;
    vert.image = greenPersonImage;
    vert.posX = 200;
    vert.posY = 200;
    auto firstVertexId = result.addVertex(vert);

    vert.shortName = "Нода 2";
    vert.name = "Соединён с 3";
    vert.backgroundColor = Qt::red;
    vert.image = redPersonImage;
    vert.posX = 500;
    vert.posY = 200;
    auto secondVertexId = result.addVertex(vert);

    vert.shortName = "Нода 3";
    vert.name = "Соединён с 2 и 4";
    vert.backgroundColor = Qt::green;
    vert.image = {};
    vert.posX = 300;
    vert.posY = 500;
    auto thirdVertexId = result.addVertex(vert);

    vert.shortName = "Нода 4";
    vert.name = "Соединён с 1 и 2";
    vert.backgroundColor = Qt::red;
    vert.image = {};
    vert.posX = 100;
    vert.posY = 400;
    auto fourthVertexId = result.addVertex(vert);

    vert.shortName = "Тест нода";
    vert.name = "Соединён с 1";
    vert.backgroundColor = QColor();
    vert.borderColor     = QColor();
    vert.image = {};
    vert.posX = 900;
    vert.posY = 400;
    auto fifthVertexId = result.addVertex(vert);

    Graph::GConnection con;

    con.name = "1-2";
    con.idFrom = firstVertexId;
    con.idTo = secondVertexId;
    con.lineColor = Qt::green;
    result.addConnection(con);

    con.name = "1-3";
    con.idFrom = firstVertexId;
    con.idTo = thirdVertexId;
    con.lineColor = Qt::green;
    result.addConnection(con);

    con.name = "2-3";
    con.idFrom = secondVertexId;
    con.idTo = thirdVertexId;
    con.lineColor = Qt::red;
    result.addConnection(con);

    con.name = "3-2";
    con.idFrom = thirdVertexId;
    con.idTo = secondVertexId;
    con.lineColor = Qt::black;
    result.addConnection(con);

    con.name = "3-4";
    con.idFrom = thirdVertexId;
    con.idTo = fourthVertexId;
    con.lineColor = Qt::black;
    result.addConnection(con);

    con.name = "4-1";
    con.idFrom = fourthVertexId;
    con.idTo = firstVertexId;
    con.lineColor = Qt::magenta;
    result.addConnection(con);

    con.name = "4-2";
    con.idFrom = fourthVertexId;
    con.idTo = secondVertexId;
    con.lineColor = Qt::magenta;
    result.addConnection(con);

    con.name = "5-1";
    con.idFrom = fifthVertexId;
    con.idTo = firstVertexId;
    con.lineColor = Qt::cyan;
    result.addConnection(con);

    return result;
}
