#ifndef GVERTEX_H
#define GVERTEX_H

#include <QIcon>
#include <QString>
#include <QPixmap>
#include <QJsonObject>

#include "graphcommon.h"

namespace Graph
{

const unsigned GRAPH_MAX_SHORTNAME_SIZE { 10 };

/**
 * @brief The GVertex class Вершина графа
 */
struct GVertex
{
    GraphCommon::graphId_t        id                  {0};                //! ID вершины
    double      posX                {0};                //! При наличии, положение на графе. Записывается после первой отрисовки
    double      posY                {0};                //! При наличии, положение на графе. Записывается после первой отрисовки

    // Основные поля
    QString     shortName           {};                 //! Краткое имя врешины (отображаемое)
    QString     name                {};                 //! Полное имя вершины
    QString     description         {};                 //! Описание вершины
    QJsonObject customProperties    {};                 //! JSON с пользовательскими свойствами

    // Отрисовка
    QColor  borderColor     {Qt::black}; //! Цвет границы
    QColor  backgroundColor {Qt::white}; //! Цвет фона
    QImage  image;              //! Изображение с альфа-каналом

    /**
     * @brief isShortnameValid Проверка корректности длины краткого наименования вершины
     * @return false если длина больше разрешённой
     */
    bool isShortnameValid() const {
        return static_cast<unsigned>(shortName.size()) < GRAPH_MAX_SHORTNAME_SIZE;
    }

    /**
     * @brief isValid Проверка на корректность данных структуры
     * @return false если объект не может использоваться в графе
     */
    bool isValid() const {
        return isShortnameValid() && (id != 0) && ((borderColor.isValid() && backgroundColor.isValid()) || !image.isNull());
    }

    /**
     * @brief tieFields     Соединить вместе поля структуры для выполнения бинарного оператора к ним
     * @param vert          Вершина, с которой вместе выполнить оператор
     * @param vertOperator  Оператор для применения
     * @return              То, что вернёт оператор
     */
    template <typename OperatorT>
    bool tieFields(const GVertex& vert, OperatorT&& vertOperator) const {
        return vertOperator(
            std::tie(id, shortName, name, description,
                     customProperties, borderColor,
                     backgroundColor),
            std::tie(vert.id, vert.shortName, vert.name, vert.description,
                     vert.customProperties, vert.borderColor,
                     vert.backgroundColor));
    }

    /**
     * @brief operator ==   Оператор равенства
     * @param oVert_        Другая вершина
     * @return              true если вершина совпадает с этой
     */
    bool operator ==(const GVertex& oVert_) const {
        return  tieFields(oVert_, std::equal_to<>{}) &&
                (std::fabs(posX - oVert_.posX) < std::numeric_limits<double>::epsilon()) &&
                (std::fabs(posY - oVert_.posY) < std::numeric_limits<double>::epsilon());
    }

    /**
     * @brief operator !=   Оператор неравенства
     * @param oVert_        Другая вершина
     * @return              true если вершина НЕ совпадает с этой
     */
    bool operator !=(const GVertex& oVert_) const {
        return  tieFields(oVert_, std::not_equal_to<>{}) ||
                (std::fabs(posX - oVert_.posX) > std::numeric_limits<double>::epsilon()) ||
                (std::fabs(posY - oVert_.posY) > std::numeric_limits<double>::epsilon());
    }
};

}

#endif // GVERTEX_H
