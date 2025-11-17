#pragma once

// Макрос для экспорта функций
#ifdef _WIN32
    #ifdef DEFAULT_PLUGIN_EXPORT
        #define DEFAULT_PLUGIN_API __declspec(dllexport)
    #else
        #define DEFAULT_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define DEFAULT_PLUGIN_API __attribute__((visibility("default")))
#endif

#include <string>

namespace Graph {
class VertexItem;
class VertexConnectionLine;
}

/**
 * @brief Основная функция плагина. Позволяет определить, какие в целом есть элементы в плагине, через точку с запятой
 */
const char* private_getLibraryItemNames();
extern "C" DEFAULT_PLUGIN_API const char* getLibraryItemNames() {
    return private_getLibraryItemNames();
}

/**
 * @brief Дополнительная к основной функция плагина. Позволяет определить типы элементов, полученных getLibraryItemNames
 */
const char* private_getItemType(const std::string& itemName);
extern "C" DEFAULT_PLUGIN_API const char* getItemType(const std::string& itemName) {
    return private_getItemType(itemName);
}


/**
 * @brief Функция плагина, позволяющая создавать вершины (ключевая для плагина вершин)
 */
Graph::VertexItem* private_createVertex(const std::string& name);
extern "C" DEFAULT_PLUGIN_API Graph::VertexItem* createVertex(const std::string& name) {
    return private_createVertex(name);
}

/**
 * @brief Функция плагина, позволяющая создавать соединения (ключевая для плагина соединений)
 */
Graph::VertexConnectionLine* private_createConnection(const std::string& name);
extern "C" DEFAULT_PLUGIN_API Graph::VertexConnectionLine* createConnection(const std::string& name) {
    return private_createConnection(name);
}

