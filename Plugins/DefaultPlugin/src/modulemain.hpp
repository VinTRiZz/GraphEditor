#pragma once

#include <GraphItems/VertexItem.h>

// Функция для создания вершины
extern "C" Graph::VertexItem* createVertex(const std::string& namespace_str, const std::string& name);
