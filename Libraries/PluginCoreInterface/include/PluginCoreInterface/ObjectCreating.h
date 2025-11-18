#pragma once

// Макрос для экспорта функций
#ifndef PLUGIN_API_EXPORT
#ifdef _WIN32
    #ifdef DEFAULT_PLUGIN_EXPORT
        #define PLUGIN_API_EXPORT __declspec(dllexport)
    #else
        #define PLUGIN_API_EXPORT __declspec(dllimport)
    #endif
#else
    #define PLUGIN_API_EXPORT __attribute__((visibility("default")))
#endif
#endif // NDEF PLUGIN_API_EXPORT

#include <string>

namespace Graph {
class VertexItem;
class VertexConnectionLine;
}

/**
 * @brief Функция плагина, позволяющая создавать вершины (ключевая для плагина вершин)
 */
extern "C" PLUGIN_API_EXPORT Graph::VertexItem* createVertex(const std::string& name);

/**
 * @brief Функция плагина, позволяющая создавать соединения (ключевая для плагина соединений)
 */
extern "C" PLUGIN_API_EXPORT Graph::VertexConnectionLine* createConnection(const std::string& name);
