#ifndef GRAPHCOMMON_H
#define GRAPHCOMMON_H

#include <QJsonDocument>
#include <QVariant>
#include <QVector>

namespace GraphCommon
{

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

template <typename InputValueType, typename OutputArrayType>
bool convertEncode(InputValueType inv, uint8_t maxDigit, std::list<OutputArrayType>& olist)
{
    olist.clear();
    while (inv > 0) {
        olist.push_front(inv % maxDigit);
        inv -= olist.front();
        inv /= maxDigit;
    }
    return inv == 0;
}

template <typename InputArrayType, typename OutputValueType>
void convertDecode(const std::list<InputArrayType>& ilist, uint8_t maxDigit, OutputValueType& oval)
{
    oval = 0;
    uint64_t curDigit = 1;
    for (auto ival = --(ilist.end()); ival != ilist.begin();) {
        oval += curDigit * (*ival);
        curDigit *= maxDigit;
        std::advance(ival, -1);
    }
    oval += curDigit * ilist.front();
}

template <typename InputValueType>
void convertEncodeChar(InputValueType inv, uint8_t maxDigit, std::string& ostr)
{
    ostr.clear();
    char bufc[2] = { '0', '\0'};
    while (inv > 0) {
        auto nextV = inv % maxDigit;
        bufc[0] = nextV > 9 ? nextV - 10 + 'a' : nextV + '0';
        ostr = std::string(bufc) + ostr;
        inv -= nextV;
        inv /= maxDigit;
    }
}

}

#endif // GRAPHCOMMON_H
