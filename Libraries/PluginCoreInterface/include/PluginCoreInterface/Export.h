#pragma once

#include "Core.h"

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

// ============================================================ //
// Получение корневого класса плагина

/**
 * @brief Получение инстанции корневого класса плагина
 * @note  Инстанция класса записывается в std::shared_ptr, поэтому лучше не удалять изнутри плагина
 */
extern "C" PLUGIN_API_EXPORT Graph::AbstractPluginCore* createPluginCore();
