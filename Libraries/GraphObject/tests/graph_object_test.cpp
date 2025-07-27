#include <gtest/gtest.h>

#include <GraphObject/Object.h>

#include <QGuiApplication>

TEST(GraphObject, ColorConversion) {
    QColor testColor (130, 224, 99, 32);

    auto encodedColor = GraphCommon::encodeColor(testColor);
    EXPECT_EQ(encodedColor.size(), 9);

    auto decodedColor = GraphCommon::decodeColor(encodedColor);
    EXPECT_EQ(testColor, decodedColor);
}

TEST(GraphObject, AddRemoveTest) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

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
        EXPECT_EQ(result.addVertex(vert), true);
    }

    auto addedVertices = result.getAllVertices();
    EXPECT_EQ(addedVertices.size(), testVertices.size());

    // На всякий
    auto vertSortPredicate = [](auto& con1, auto& con2){
        return con1.id < con2.id;
    };
    addedVertices.sort(vertSortPredicate);
    testVertices.sort(vertSortPredicate);

    EXPECT_EQ(addedVertices, testVertices);

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
        EXPECT_EQ(result.addConnection(con), true);
    }

    auto addedConnections = result.getAllConnections();
    EXPECT_EQ(addedConnections.size(), testConnections.size());

    // На всякий
    auto conSortPredicate = [](auto& con1, auto& con2){
        return con1.idFrom > con2.idFrom;
    };
    auto conSortPredicate2 = [](auto& con1, auto& con2){
        return con1.idTo > con2.idTo;
    };
    std::sort(addedConnections.begin(), addedConnections.end(), conSortPredicate2);
    std::stable_sort(addedConnections.begin(), addedConnections.end(), conSortPredicate);

    std::sort(testConnections.begin(), testConnections.end(), conSortPredicate2);
    std::stable_sort(testConnections.begin(), testConnections.end(), conSortPredicate);

    EXPECT_EQ(addedConnections, testConnections);
}
