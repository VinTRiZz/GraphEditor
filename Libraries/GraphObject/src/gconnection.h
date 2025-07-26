#ifndef GCONNECTION_H
#define GCONNECTION_H

#include <QString>
#include <QColor>
#include <QJsonObject>

#include <math.h>

#include "graphcommon.h"

#include <Common/Logging.h>

namespace Graph
{

/**
 * @brief The GConnection class Соединение между вершинами графа
 */
struct GConnection
{
    GraphCommon::graphId_t    idFrom              {0};            //! ID из которого исходит ребро
    GraphCommon::graphId_t    idTo                {0};            //! ID в которое входит ребро
    double      connectionWeight    {0};            //! Вес ребра
    QString     name                {};             //! Название ребра
    QColor      lineColor           {Qt::black};    //! Цвет ребра
    QJsonObject customProperties;

    /**
     * @brief isValid Проверка на корректность данных структуры
     * @return false если объект не может быть использован в графе
     */
    bool isValid() const {
        return (idFrom != idTo) && lineColor.isValid();
    }

    /**
     * @brief tieFields     Соединить вместе поля структуры для выполнения бинарного оператора к ним
     * @param vert          Вершина, с которой вместе выполнить оператор
     * @param vertOperator  Оператор для применения
     * @return              То, что вернёт оператор
     */
    template <typename OperatorT>
    bool tieFields(const GConnection& cCon_, OperatorT&& vertOperator) const {
        return vertOperator(
            std::tie(idFrom, idTo,
                     name, lineColor),
            std::tie(cCon_.idFrom, cCon_.idTo,
                     cCon_.name, cCon_.lineColor));
    }

    /**
     * @brief operator ==   Оператор равенства
     * @param oVert_        Другая вершина
     * @return              true если вершина совпадает с этой
     */
    bool operator ==(const GConnection& oCon_) const {
        auto weightEquality = (fabs(oCon_.connectionWeight - connectionWeight) < 1e-6);
        auto res = tieFields(oCon_, std::equal_to<>{}) && weightEquality;
        return res;
    }

    /**
     * @brief operator !=   Оператор неравенства
     * @param oVert_        Другая вершина
     * @return              true если вершина НЕ совпадает с этой
     */
    bool operator !=(const GConnection& oCon_) const {
        auto weightEquality = (fabs(oCon_.connectionWeight - connectionWeight) < 1e-6);
        return tieFields(oCon_, std::not_equal_to<>{}) && !weightEquality;
    }
};

}
#endif // GCONNECTION_H
