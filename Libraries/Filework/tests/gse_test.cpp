#include <gtest/gtest.h>

#include <GraphObject/Object.h>

#include <QGuiApplication>

#include <QFile>

#include "../src/gse_format.h"

TEST(FormatSaving, GSE_Format) {
    Graph::GraphObject savedGraph;

    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    savedGraph.setName("Test example graph");
    savedGraph.setDescription("Example description");
    savedGraph.setCreateTime(QDateTime::currentDateTime());
    savedGraph.setEditTime(QDateTime::currentDateTime());

    savedGraph.setCustomValue("Test value", "test data");
    savedGraph.setCustomValue("Test value 2", "test data 2");

    Graph::GraphObject result;

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
        result.addVertex(vert);
    }

    // Vector cuz return type is vector
    std::vector<Graph::GConnection> testConnections;

    Graph::GConnection con;

    con.name = "1-2";
    con.idFrom = 50;
    con.idTo = 51;
    con.lineColor = Qt::green;
    testConnections.push_back(con);

    con.name = "1-3";
    con.idFrom = 50;
    con.idTo = 52;
    con.lineColor = Qt::green;
    testConnections.push_back(con);

    con.name = "2-3";
    con.idFrom = 51;
    con.idTo = 52;
    con.lineColor = Qt::red;
    testConnections.push_back(con);

    con.name = "3-2";
    con.idFrom = 52;
    con.idTo = 51;
    con.lineColor = Qt::black;
    testConnections.push_back(con);

    con.name = "3-4";
    con.idFrom = 52;
    con.idTo = 53;
    con.lineColor = Qt::black;
    testConnections.push_back(con);

    con.name = "4-1";
    con.idFrom = 53;
    con.idTo = 50;
    con.lineColor = Qt::magenta;
    testConnections.push_back(con);

    con.name = "4-2";
    con.idFrom = 53;
    con.idTo = 51;
    con.lineColor = Qt::magenta;
    testConnections.push_back(con);

    con.name = "5-1";
    con.idFrom = 54;
    con.idTo = 50;
    con.lineColor = Qt::cyan;
    testConnections.push_back(con);

    for (auto& con : testConnections) {
        result.addConnection(con);
    }

    Filework::GSE_Format gseFormat;

    auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на затирание данных)
    gseFormat.setGraph(&graphCopy);

    QString testTargetPathEmptyExtension = "/tmp/GraphEditorSaveTest";
    auto testTargetPath = testTargetPathEmptyExtension + ".gse";

    // С расширением
    EXPECT_EQ(gseFormat.save(testTargetPath), true);
    EXPECT_EQ(graphCopy, savedGraph);

    Graph::GraphObject loadedGraph;
    gseFormat.setGraph(&loadedGraph);
    EXPECT_EQ(gseFormat.load(testTargetPath), true);
    QFile::remove(testTargetPath);
    EXPECT_EQ(loadedGraph, graphCopy);

    // Без расширения
    EXPECT_EQ(gseFormat.save(testTargetPathEmptyExtension), true);
    EXPECT_EQ(graphCopy, savedGraph);

    loadedGraph = Graph::GraphObject();
    gseFormat.setGraph(&loadedGraph);
    EXPECT_EQ(gseFormat.load(testTargetPathEmptyExtension), true);
    QFile::remove(testTargetPathEmptyExtension);
    EXPECT_EQ(loadedGraph, graphCopy);
}
