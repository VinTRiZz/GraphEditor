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
// Для редактирования свойств виджетов
class PluginItemPropertyWidget;

// Для работы с режимом, определённым в плагине
class PluginInteractionWidget;

// Для работы с конфигурацией плагина
class PluginConfigurationWidget;

class PluginObjectItnterface;
}

/**
 * @brief Функция плагина, позволяющая создать виджет для настройки работы плагина
 * @note    Виджет не удаляется приложением, его можно переиспользовать
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginConfigurationWidget* getConfigurationEditor();

/**
 * @brief Функция плагина для получения виджета редактора свойств конкретного объекта сцены
 * @note    Виджет не удаляется приложением, его можно переиспользовать
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginItemPropertyWidget* getPropertyEditor(Graph::PluginObjectItnterface* pTarget);

/**
 * @brief Функция плагина для создания виджета-интерактора с плагином (например, для запуска алгоритма в алгоритм. плагине)
 * @note    Виджет не удаляется приложением, его можно переиспользовать
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginInteractionWidget* getInteractor();
