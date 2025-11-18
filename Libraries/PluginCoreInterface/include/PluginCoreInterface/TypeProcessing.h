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
 * @brief Функция плагина, позволяющая получить количество доступных типов объектов
 * @note    Возвращает число доступных типов, от 0 до N,
 *          Любое число от 0 до N не включительно должно быть валидным
 */
extern "C" PLUGIN_API_EXPORT long long getTypeCount();

/**
 * @brief Функция плагина, позволяющая получить указатель на объект std::string с названием объекта
 * @note    Память, выделенная под указатель, переходит во владение вызывающего.
 *
 * @note    Желательно использовать названия в UTF-8
 */
extern "C" PLUGIN_API_EXPORT std::string* getItemName(long long itemId);

/**
 * @brief Функция плагина, позволяющая получить указатель на объект std::string с названием типа объекта
 * @note    Память, выделенная под указатель, переходит во владение вызывающего.
 *
 * @note    Желательно использовать названия в UTF-8, в соответствии с доступными
 *          в граф редакторе типами объектов
 */
extern "C" PLUGIN_API_EXPORT std::string* getTypeString(long long itemId);
