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
class PluginModeWidget;

// Для работы с конфигурацией плагина
class PluginConfigurationWidget;
}

namespace ObjectItems {
class BasicItem;
}

/**
 * @brief Функция плагина, позволяющая создать виджет для настройки работы плагина
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginConfigurationWidget* createConfigurationEditor();

/**
 * @brief Функция плагина для создания редактора свойств конкретного объекта сцены
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginItemPropertyWidget* createPropertyEditor(ObjectItems::BasicItem* pTarget);

/**
 * @brief Функция плагина для создания виджета-конфигуратора режима
 */
extern "C" PLUGIN_API_EXPORT Graph::PluginModeWidget* createMode(int modeType);
