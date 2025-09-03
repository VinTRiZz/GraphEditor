#include "testgenerators.h"

#include <QColor>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <ctime>
#include <random>

namespace Graph::TestGenerators {

// Генератор случайных строк
QString randomString(int minLength = 3, int maxLength = 20) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng(std::time(nullptr));

    std::uniform_int_distribution<int> lenDist(minLength, maxLength);
    std::uniform_int_distribution<int> charDist(0, sizeof(alphanum) - 2);

    int length = lenDist(rng);
    QString result;
    result.reserve(length);

    for (int i = 0; i < length; ++i) {
        result.append(alphanum[charDist(rng)]);
    }

    return result;
}

// Генератор случайных цветов
QColor randomColor(bool withAlpha = false) {
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<int> dist(0, 255);

    return QColor(dist(rng), dist(rng), dist(rng), withAlpha ? dist(rng) : 255);
}

// Генератор случайных изображений
QImage randomImage() {
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<int> sizeDist(16, 64);
    std::uniform_int_distribution<int> colorDist(0, 255);
    std::uniform_int_distribution<int> alphaDist(50, 255);

    int size = sizeDist(rng);
    QImage image(size, size, QImage::Format_ARGB32);

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int alpha = (x == 0 || y == 0 || x == size - 1 || y == size - 1)
                            ? 255
                            : alphaDist(rng);
            image.setPixelColor(
                x, y,
                QColor(colorDist(rng), colorDist(rng), colorDist(rng), alpha));
        }
    }

    return image;
}

// Генератор случайных JSON-объектов
QJsonObject randomJsonObject(int maxDepth = 2) {
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<int> typeDist(0, 5);
    std::uniform_int_distribution<int> countDist(1, 5);
    std::uniform_real_distribution<double> numDist(-100.0, 100.0);
    std::uniform_int_distribution<int> boolDist(0, 1);

    QJsonObject obj;
    if (maxDepth <= 0)
        return obj;

    int itemCount = countDist(rng);
    for (int i = 0; i < itemCount; ++i) {
        QString key = "prop_" + randomString(3, 8);

        switch (typeDist(rng)) {
            case 0:  // String
                obj[key] = randomString(2, 15);
                break;
            case 1:  // Number
                obj[key] = numDist(rng);
                break;
            case 2:  // Boolean
                obj[key] = boolDist(rng) == 1;
                break;
            case 3:  // Array
            {
                QJsonArray arr;
                int arrSize = countDist(rng);
                for (int j = 0; j < arrSize; ++j) {
                    arr.append(numDist(rng));
                }
                obj[key] = arr;
            } break;
            case 4:  // Nested object
                if (maxDepth > 1) {
                    obj[key] = randomJsonObject(maxDepth - 1);
                }
                break;
            case 5:  // Null
                obj[key] = QJsonValue::Null;
                break;
        }
    }

    return obj;
}

PMaintainer createTestGraph() {
    auto testGeneratedGraph = Graph::GraphMaintainer::createInstance();

    testGeneratedGraph->setName("Test example graph");
    testGeneratedGraph->setDescription("Example description");
    testGeneratedGraph->setCreateTime(QDateTime::currentDateTime());
    testGeneratedGraph->setEditTime(QDateTime::currentDateTime());

    testGeneratedGraph->setCustomValue("Test value", "test data");
    testGeneratedGraph->setCustomValue("Test value 2", "test data 2");

    auto redPersonIcon = QIcon(":/common/images/vertexicons/person/red.svg");
    auto redPersonImage = redPersonIcon.pixmap(500).toImage();

    auto greenPersonIcon =
        QIcon(":/common/images/vertexicons/person/green.svg");
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
    vert.borderColor = QColor();
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

Graph::GVertex randomVertex(GraphCommon::graphId_t minId,
                            GraphCommon::graphId_t maxId) {
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_real_distribution<double> posDist(0.0, 1000.0);
    std::uniform_int_distribution<GraphCommon::graphId_t> idDist(minId, maxId);

    Graph::GVertex vertex;
    vertex.id = idDist(rng);
    vertex.posX = posDist(rng);
    vertex.posY = posDist(rng);
    vertex.shortName = randomString(1, Graph::GRAPH_MAX_SHORTNAME_SIZE);
    vertex.name = randomString(5, 50);
    vertex.description = randomString(0, 200);
    vertex.borderColor = randomColor(true);
    vertex.backgroundColor = randomColor(true);
    vertex.image = randomImage();
    vertex.customProperties = randomJsonObject();

    return vertex;
}

// Генератор случайных соединений
Graph::GConnection randomConnection(
    const std::vector<GraphCommon::graphId_t>& existingIds,
    bool allowSelfConnect) {
    static std::mt19937 rng(std::time(nullptr));

    if (existingIds.size() < 2) {
        throw std::invalid_argument("Need at least 2 vertices for connections");
    }

    std::uniform_real_distribution<double> weightDist(0.0, 100.0);
    std::uniform_int_distribution<size_t> indexDist(0, existingIds.size() - 1);

    Graph::GConnection conn;

    // Выбираем две разные вершины
    size_t fromIdx = indexDist(rng);
    size_t toIdx;
    do {
        toIdx = indexDist(rng);
    } while (!allowSelfConnect && fromIdx == toIdx);

    conn.idFrom = existingIds[fromIdx];
    conn.idTo = existingIds[toIdx];
    conn.connectionWeight = weightDist(rng);
    conn.name = randomString(2, 30);
    conn.lineColor = randomColor(true);
    conn.customProperties = randomJsonObject();

    return conn;
}

PMaintainer createRandomGraph() {
    auto testGeneratedGraph = Graph::GraphMaintainer::createInstance();

    testGeneratedGraph->setName(randomString());
    testGeneratedGraph->setDescription(randomString());
    testGeneratedGraph->setCreateTime(QDateTime::currentDateTime());
    testGeneratedGraph->setEditTime(QDateTime::currentDateTime());

    testGeneratedGraph->setCustomValue(randomString(), randomString());
    testGeneratedGraph->setCustomValue(randomString(), randomString());

    auto& targetGraph = testGeneratedGraph->getObject();
    for (int i = 0; i < 100; ++i) {
        targetGraph.addVertex(randomVertex());
    }

    auto allVertices = targetGraph.getAllVertices();
    std::vector<GraphCommon::graphId_t> existingVertices;
    existingVertices.reserve(allVertices.size());
    for (auto& vert : allVertices) {
        existingVertices.push_back(vert.id);
    }
    for (int i = 0; i < 30; ++i) {
        targetGraph.addConnection(randomConnection(existingVertices));
    }

    return testGeneratedGraph;
}

}  // namespace Graph::TestGenerators
