#ifndef GCONNECTION_H
#define GCONNECTION_H

#include <AppInfrastructure/CommonFunctions.h>
#include <Components/Logger/Logger.h>
#include <math.h>

#include <QColor>
#include <QJsonObject>
#include <QString>

#include "graphcommon.h"

namespace Graph {

/**
 * @brief The GConnection class Соединение между вершинами графа
 */
struct GConnection {
    GraphCommon::graphId_t idFrom{0};  //! ID из которого исходит ребро
    GraphCommon::graphId_t idTo{0};    //! ID в которое входит ребро
    double connectionWeight{0};        //! Вес ребра
    QString name{};                    //! Название ребра
    QColor lineColor{Qt::black};       //! Цвет ребра

    /**
     * @brief isValid   Проверка на корректность данных структуры
     * @return          false если объект не может быть использован в графе
     */
    bool isValid() const { return idFrom != idTo; }

    /**
     * @brief applyOperator Соединить вместе поля структуры для выполнения
     * бинарного оператора к ним
     * @param cCon_         Соединение, с которым сравнивать
     * @param conOperator   Оператор для применения
     * @return              То, что вернёт оператор
     */
    template<typename OperatorT>
    bool applyOperator(const GConnection& cCon_,
                       OperatorT&& conOperator) const {
        return conOperator(std::tie(idFrom, idTo, name),
                           std::tie(cCon_.idFrom, cCon_.idTo, cCon_.name));
    }

    /**
     * @brief operator ==   Оператор равенства
     * @param oVert_        Другая вершина
     * @return              true если вершина совпадает с этой
     */
    bool operator==(const GConnection& oCon_) const;

    /**
     * @brief operator !=   Оператор неравенства
     * @param oVert_        Другая вершина
     * @return              true если вершина НЕ совпадает с этой
     */
    bool operator!=(const GConnection& oCon_) const;
};

}  // namespace Graph
#endif  // GCONNECTION_H
