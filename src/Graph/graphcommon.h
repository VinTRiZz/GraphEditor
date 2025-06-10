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

const QString DB_GRAPH_PROPS_TABLENAME          {"graph_properties"};
const QString DB_GRAPH_PROPS_CREATEQUERY = QString(R"(
CREATE TABLE IF NOT EXISTS %0 (
id INTEGER PRIMARY KEY,
prop_name TEXT UNIQUE NOT NULL,
prop_value TEXT
);
)").arg(DB_GRAPH_PROPS_TABLENAME);

const QString DB_GRAPH_VERTICES_TABLENAME       {"vertices"};
const QString DB_GRAPH_VERTICES_CREATEQUERY = QString(R"(
CREATE TABLE IF NOT EXISTS %0 (
    id              INTEGER PRIMARY KEY,
    posx            INTEGER NOT NULL,
    posy            INTEGER NOT NULL,
    short_name      TEXT NOT NULL,
    name            TEXT,
    description     TEXT,
    custom_props    TEXT,
    color_rgba      TEXT, -- R-G-B-A in hex, example: 255 003 166 130 -> ff 03 a6 82
    bgr_color_rgba  TEXT, -- R-G-B-A in hex, example: 255 003 166 130 -> ff 03 a6 82
    pxmap           TEXT  -- Pixmap as PNG
);
)").arg(DB_GRAPH_VERTICES_TABLENAME);

const QString DB_GRAPH_CONNECTIONS_TABLENAME    {"connections"};
const QString DB_GRAPH_CONNECTIONS_CREATEQUERY = QString(R"(
CREATE TABLE IF NOT EXISTS %0 (
    id          INTEGER PRIMARY KEY,
    idFrom      INTEGER NOT NULL,
    idTo        INTEGER NOT NULL,
    weight      FLOAT DEFAULT 0,
    name        TEXT,
    color_rgba  TEXT, -- R-G-B-A in hex, example: 255 003 166 130 -> ff 03 a6 82
    is_directed BOOLEAN DEFAULT FALSE,

    FOREIGN KEY (idFrom) REFERENCES %1(id) ON DELETE CASCADE,
    FOREIGN KEY (idTo) REFERENCES %1(id) ON DELETE CASCADE
);
)").arg(DB_GRAPH_CONNECTIONS_TABLENAME, DB_GRAPH_VERTICES_TABLENAME);


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
