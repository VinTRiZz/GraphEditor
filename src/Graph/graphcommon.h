#ifndef GRAPHCOMMON_H
#define GRAPHCOMMON_H

#include <QJsonDocument>
#include <QVariant>
#include <QVector>

namespace GraphCommon
{

const QString DATE_CONVERSION_FORMAT = "HH:mm_dd.MM.yyyy";          //! Формат конвертирования даты в строку
const QString DATE_DISPLAY_CONVERSION_FORMAT = "HH:mm dd.MM.yyyy";  //! Формат конвертирования даты в строку на показ

/**
 * @brief showInfo Функция, показывающая месседж бокс
 * @param text Текст информационного сообщения
 */
void showInfo(const QString& text);

/**
 * @brief showWarning Функция, показывающая месседж бокс
 * @param text Текст сообщения "Внимание"
 */
void showWarning(const QString& text);

/**
 * @brief showError  Функция, показывающая месседж бокс
 * @param text Текст ошибки
 */
void showError(const QString& text);

/**
 * @brief The GraphProperty enum Обычные для всех свойства графа
 */
enum GraphProperty
{
    GRAPHPROPERTY_NAME,         //! Название графа
    GRAPHPROPERTY_DESCRIPTION,  //! Описание графа
    GRAPHPROPERTY_CREATETIME,   //! Время создания
    GRAPHPROPERTY_EDITTIME,     //! Время последнего изменения
};

}

#endif // GRAPHCOMMON_H
