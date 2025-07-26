#include <gtest/gtest.h>

#include <GraphObject/Object.h>

#include <QGuiApplication>

#include <QFile>

#include "../src/savemaster.h"

TEST(FormatSaving, InterfaceSaveTest) {
    Graph::GraphObject savedGraph;

    savedGraph.setName("Test example graph");
    savedGraph.setDescription("Example description");
    savedGraph.setCreateTime(QDateTime::currentDateTime());
    savedGraph.setEditTime(QDateTime::currentDateTime());

    savedGraph.setCustomValue("Test value", "test data");
    savedGraph.setCustomValue("Test value 2", "test data 2");

    auto redPersonIcon = QIcon(":/common/images/vertexicons/person/red.svg");
    auto redPersonImage = redPersonIcon.pixmap(500).toImage();

    auto greenPersonIcon = QIcon(":/common/images/vertexicons/person/green.svg");
    auto greenPersonImage = greenPersonIcon.pixmap(500).toImage();

    Graph::GVertex vert;
    vert.shortName = "Vertex 1";
    vert.backgroundColor = Qt::red;
    vert.posX = 100;
    vert.posY = 100;
    vert.image = redPersonImage;
    savedGraph.addVertex(vert);

    vert.shortName = "Vertex 2";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 100;
    vert.posY = 300;
    vert.image = greenPersonImage;
    savedGraph.addVertex(vert);

    vert.shortName = "Vertex 3";
    vert.backgroundColor = Qt::red;
    vert.posX = 300;
    vert.posY = 100;
    vert.image = redPersonImage;
    savedGraph.addVertex(vert);

    vert.shortName = "Vertex 4";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 300;
    vert.posY = 300;
    vert.image = redPersonImage;
    savedGraph.addVertex(vert);

    Graph::GConnection con;
    con.name = "Connection 1";
    con.idFrom = 1;
    con.idTo = 2;
    con.lineColor = Qt::red;
    con.connectionWeight = 712;
    savedGraph.addConnection(con);

    con.name = "Connection 2";
    con.idFrom = 2;
    con.idTo = 3;
    con.lineColor = Qt::magenta;
    con.connectionWeight = 152;
    savedGraph.addConnection(con);

    con.name = "Connection 3";
    con.idFrom = 1;
    con.idTo = 3;
    con.lineColor = Qt::yellow;
    con.connectionWeight = 23;
    savedGraph.addConnection(con);

    con.name = "Connection 4";
    con.idFrom = 1;
    con.idTo = 4;
    con.lineColor = Qt::green;
    con.connectionWeight = 31;
    savedGraph.addConnection(con);

    con.name = "Connection 5";
    con.idFrom = 3;
    con.idTo = 1;
    con.lineColor = Qt::cyan;
    con.connectionWeight = 103;
    savedGraph.addConnection(con);

    SaveMaster saver;

    auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на затирание данных)

    QString testTargetPathEmptyExtension = "/tmp/GraphEditorSaveTest";

    EXPECT_EQ(saver.save(testTargetPathEmptyExtension, &savedGraph), true);
    EXPECT_EQ(graphCopy, savedGraph);

    Graph::GraphObject loadedGraph;
    EXPECT_EQ(saver.load(testTargetPathEmptyExtension, &loadedGraph), true);
    QFile::remove(saver.formatToDefaultPath(testTargetPathEmptyExtension));
    EXPECT_EQ(loadedGraph, graphCopy);
}
