#include "testgenerators.h"

namespace Graph::TestGenerators
{

PMaintainer createTestGraph()
{
    auto testGeneratedGraph = Graph::GraphMaintainer::createInstance();

    testGeneratedGraph->setName("Test example graph");
    testGeneratedGraph->setDescription("Example description");
    testGeneratedGraph->setCreateTime(QDateTime::currentDateTime());
    testGeneratedGraph->setEditTime(QDateTime::currentDateTime());

    testGeneratedGraph->setCustomValue("Test value", "test data");
    testGeneratedGraph->setCustomValue("Test value 2", "test data 2");

    auto redPersonIcon = QIcon(":/common/images/vertexicons/person/red.svg");
    auto redPersonImage = redPersonIcon.pixmap(500).toImage();

    auto greenPersonIcon = QIcon(":/common/images/vertexicons/person/green.svg");
    auto greenPersonImage = greenPersonIcon.pixmap(500).toImage();

    std::list<Graph::GVertex> testVertices;

    Graph::GVertex vert;
    vert.id = 50;
    vert.shortName = "Нода 1";
    vert.name = "Соединён с 2 и 3";
    vert.backgroundColor = Qt::green;
    vert.image = greenPersonImage;
    vert.posX = 200;
    vert.posY = 200;
    testVertices.push_back(vert);

    vert.id = 51;
    vert.shortName = "Нода 2";
    vert.name = "Соединён с 3";
    vert.backgroundColor = Qt::red;
    vert.image = redPersonImage;
    vert.posX = 500;
    vert.posY = 200;
    testVertices.push_back(vert);

    vert.id = 52;
    vert.shortName = "Нода 3";
    vert.name = "Соединён с 2 и 4";
    vert.backgroundColor = Qt::green;
    vert.image = {};
    vert.posX = 300;
    vert.posY = 500;
    testVertices.push_back(vert);

    vert.id = 53;
    vert.shortName = "Нода 4";
    vert.name = "Соединён с 1 и 2";
    vert.backgroundColor = Qt::red;
    vert.image = {};
    vert.posX = 100;
    vert.posY = 400;
    testVertices.push_back(vert);

    vert.id = 54;
    vert.shortName = "Тест нода";
    vert.name = "Соединён с 1";
    vert.backgroundColor = QColor();
    vert.borderColor     = QColor();
    vert.image = {};
    vert.posX = 900;
    vert.posY = 400;
    testVertices.push_back(vert);

    for (auto& vert : testVertices) {
        testGeneratedGraph->getObject().addVertex(vert);
    }

    // Vector cuz return type is vector
    std::vector<Graph::GConnection> testConnections;

    Graph::GConnection tmpCon;

    tmpCon.name = "1-2";
    tmpCon.idFrom = 50;
    tmpCon.idTo = 51;
    tmpCon.lineColor = Qt::green;
    testConnections.push_back(tmpCon);

    tmpCon.name = "1-3";
    tmpCon.idFrom = 50;
    tmpCon.idTo = 52;
    tmpCon.lineColor = Qt::green;
    testConnections.push_back(tmpCon);

    tmpCon.name = "2-3";
    tmpCon.idFrom = 51;
    tmpCon.idTo = 52;
    tmpCon.lineColor = Qt::red;
    testConnections.push_back(tmpCon);

    tmpCon.name = "3-2";
    tmpCon.idFrom = 52;
    tmpCon.idTo = 51;
    tmpCon.lineColor = Qt::black;
    testConnections.push_back(tmpCon);

    tmpCon.name = "3-4";
    tmpCon.idFrom = 52;
    tmpCon.idTo = 53;
    tmpCon.lineColor = Qt::black;
    testConnections.push_back(tmpCon);

    tmpCon.name = "4-1";
    tmpCon.idFrom = 53;
    tmpCon.idTo = 50;
    tmpCon.lineColor = Qt::magenta;
    testConnections.push_back(tmpCon);

    tmpCon.name = "4-2";
    tmpCon.idFrom = 53;
    tmpCon.idTo = 51;
    tmpCon.lineColor = Qt::magenta;
    testConnections.push_back(tmpCon);

    tmpCon.name = "5-1";
    tmpCon.idFrom = 54;
    tmpCon.idTo = 50;
    tmpCon.lineColor = Qt::cyan;
    testConnections.push_back(tmpCon);

    for (auto& con : testConnections) {
        testGeneratedGraph->getObject().addConnection(con);
    }
    return testGeneratedGraph;
}

}
