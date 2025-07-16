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


const QString DATE_CONVERSION_FORMAT = "HH:mm:ss_dd.MM.yyyy";          //! Формат конвертирования даты в строку
const QString DATE_DISPLAY_CONVERSION_FORMAT = "HH:mm:ss dd.MM.yyyy";  //! Формат конвертирования даты в строку на показ

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
    posx            FLOAT NOT NULL,
    posy            FLOAT NOT NULL,
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

}

#endif // GRAPHCOMMON_H
