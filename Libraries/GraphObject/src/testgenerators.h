#ifndef TESTGENERATORS_H
#define TESTGENERATORS_H

#include "graphmaintaner.h"

namespace Graph::TestGenerators {

/**
 * @brief createTestGraph   Создаёт вполне конкретный граф для тестов
 * @return                  Тестовый граф
 */
PMaintainer createTestGraph();

/**
 * @brief randomVertex  Создаёт полностью случайную вершину графа
 * @param minId         Минимальный разрешённый ID
 * @param maxId         Максимальный разрешённый ID
 * @return              Случайная вершина
 */
Graph::GVertex randomVertex(GraphCommon::graphId_t minId = 1,
                            GraphCommon::graphId_t maxId = 1000);

/**
 * @brief randomConnection  Создаёт полностью случайное соединение
 * @param existingIds       Имеющиеся ID вершин
 * @param allowSelfConnect  Флаг разрешения соединять вершины сами на себя
 * @return                  Случайное соединение
 */
Graph::GConnection randomConnection(
    const std::vector<GraphCommon::graphId_t>& existingIds,
    bool allowSelfConnect = false);

/**
 * @brief createRandomGraph Создаёт граф с совершенно случайными данными, каждый
 * раз разный
 * @return                  Случайный граф
 */
PMaintainer createRandomGraph();

}  // namespace Graph::TestGenerators

#endif  // TESTGENERATORS_H
