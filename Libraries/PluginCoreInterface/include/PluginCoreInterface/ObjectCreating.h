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
class PluginObjectItnterface;
}

/**
 * @brief Функция плагина, позволяющая создавать объект по его имени
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginObjectItnterface* createObject(const std::string& name);
