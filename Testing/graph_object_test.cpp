#include <gtest/gtest.h>

#include <GraphObject/Object.h>

#include <QGuiApplication>

TEST(GraphObject, VertexWork) {

    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    Graph::GraphObject result;

    auto redPersonIcon = QIcon(":/common/images/vertexicons/person/red.svg");
    auto redPersonImage = redPersonIcon.pixmap(500).toImage();

    auto greenPersonIcon = QIcon(":/common/images/vertexicons/person/green.svg");
    auto greenPersonImage = greenPersonIcon.pixmap(500).toImage();

    Graph::GVertex vert;
    vert.id = 50;
    vert.shortName = "Нода 1";
    vert.name = "Соединён с 2 и 3";
    vert.backgroundColor = Qt::green;
    vert.image = greenPersonImage;
    vert.posX = 200;
    vert.posY = 200;
    EXPECT_EQ(result.addVertex(vert), true);

    vert.id = 51;
    vert.shortName = "Нода 2";
    vert.name = "Соединён с 3";
    vert.backgroundColor = Qt::red;
    vert.image = redPersonImage;
    vert.posX = 500;
    vert.posY = 200;
    EXPECT_EQ(result.addVertex(vert), true);

    vert.id = 52;
    vert.shortName = "Нода 3";
    vert.name = "Соединён с 2 и 4";
    vert.backgroundColor = Qt::green;
    vert.image = {};
    vert.posX = 300;
    vert.posY = 500;
    EXPECT_EQ(result.addVertex(vert), true);

    vert.id = 53;
    vert.shortName = "Нода 4";
    vert.name = "Соединён с 1 и 2";
    vert.backgroundColor = Qt::red;
    vert.image = {};
    vert.posX = 100;
    vert.posY = 400;
    EXPECT_EQ(result.addVertex(vert), true);

    vert.id = 54;
    vert.shortName = "Тест нода";
    vert.name = "Соединён с 1";
    vert.backgroundColor = QColor();
    vert.borderColor     = QColor();
    vert.image = {};
    vert.posX = 900;
    vert.posY = 400;
    EXPECT_EQ(result.addVertex(vert), true);

    Graph::GConnection con;

    con.name = "1-2";
    con.idFrom = 50;
    con.idTo = 51;
    con.lineColor = Qt::green;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "1-3";
    con.idFrom = 50;
    con.idTo = 52;
    con.lineColor = Qt::green;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "2-3";
    con.idFrom = 51;
    con.idTo = 52;
    con.lineColor = Qt::red;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "3-2";
    con.idFrom = 52;
    con.idTo = 51;
    con.lineColor = Qt::black;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "3-4";
    con.idFrom = 52;
    con.idTo = 53;
    con.lineColor = Qt::black;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "4-1";
    con.idFrom = 53;
    con.idTo = 50;
    con.lineColor = Qt::magenta;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "4-2";
    con.idFrom = 53;
    con.idTo = 51;
    con.lineColor = Qt::magenta;
    EXPECT_EQ(result.addConnection(con), true);

    con.name = "5-1";
    con.idFrom = 54;
    con.idTo = 50;
    con.lineColor = Qt::cyan;
    EXPECT_EQ(result.addConnection(con), true);

    EXPECT_EQ(result.getAllVertices().size(), 5);
    EXPECT_EQ(result.getAllConnections().size(), 8);
}
