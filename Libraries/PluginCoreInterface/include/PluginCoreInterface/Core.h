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

/**
 * @brief Функция, позволяющая получить указатель на строку с текстом ошибки
 * @note Выделенная под указатель память переходит во владение вызывающего
 */
extern "C" PLUGIN_API_EXPORT std::string* getErrorText();

/**
 * @brief Инициализация плагина
 */
extern "C" PLUGIN_API_EXPORT int initPlugin();

/**
 * @brief Восстановление состояния после деинициализации плагина (вызывается всегда после инициализации)
 */
extern "C" PLUGIN_API_EXPORT int restoreState();

/**
 * @brief Деинициализация плагина
 */
extern "C" PLUGIN_API_EXPORT int deinitPlugin();
