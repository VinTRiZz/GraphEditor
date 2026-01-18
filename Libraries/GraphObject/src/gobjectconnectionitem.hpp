#pragma once

#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>

#include <GraphObject/PluginObjectInterface.h>

namespace Graph {

class GObjectItem;
class GObjectConnectionItem :
        public ObjectItems::BasicItem,
        public PluginObjectInterface {
public:
    explicit GObjectConnectionItem(QGraphicsItem* parent = nullptr);
    ~GObjectConnectionItem();

    virtual QJsonObject toJson() const override;
    virtual bool fromJson(const QJsonObject& arr) override;

    void setVertexFrom(GObjectItem* pVertexFrom);
    GObjectItem* getVertexFrom() const;

    void setVertexTo(GObjectItem* pVertexTo);
    GObjectItem* getVertexTo() const;

    std::pair<graphId_t, graphId_t> getVertexIds() const;

    /**
     * @brief setLineItem Задать новый айтем соединения. Удаляет предыдущий
     * @param pLine
     */
    void setLineItem(ObjectItems::AbstractConnectionLine* pLine);
    ObjectItems::AbstractConnectionLine* getLineItem() const;

private:
    GObjectItem* m_fromVertex{nullptr};
    GObjectItem* m_toVertex{nullptr};

    graphId_t m_loadedFromId {};
    graphId_t m_loadedToId {};

    bool m_isStraightLine {false};

    ObjectItems::AbstractConnectionLine* m_connectionLine {nullptr};

    void updateLine();
};

template <typename LineBaseT>
class ConnectionLineBase :
        public LineBaseT,
        public PluginObjectInterface
{
public:
    using LineBaseT::LineBaseT;

    virtual QJsonObject toJson() const override {
        auto res = PluginObjectInterface::toJson();

        auto pThis = static_cast<const LineBaseT*>(this);

        QJsonObject selfJson;
        selfJson["direction"] = pThis->getDirection();
        selfJson["arrowAngle"] = pThis->getArrowAngle();
        selfJson["arrowIsFilled"] = pThis->getIsArrowFilled();
        selfJson["color"] = pThis->getLinePen().color().name();

        auto selfLine = pThis->getLine();
        selfJson["line"] = QString("%0;%1;%2;%3").arg(  QString::number(selfLine.p1().x()),
                                                        QString::number(selfLine.p1().y()),
                                                        QString::number(selfLine.p2().x()),
                                                        QString::number(selfLine.p2().y()));

        auto selfSize = pThis->getArrowHeight();
        selfJson["arrowHeight"] = QString("%0").arg(QString::number(selfSize));

        res["ConnectionLineBase"] = selfJson;

        return res;
    }

    virtual bool fromJson(const QJsonObject& arr) override {
        auto res = PluginObjectInterface::fromJson(arr);

        auto pThis = static_cast<LineBaseT*>(this);

        auto selfJson = arr["ConnectionLineBase"].toObject();
        pThis->setDirection(ObjectItems::LineDirectionType(selfJson["direction"].toInt()));
        pThis->setArrowAngle(ObjectItems::LineAngleType(selfJson["arrowAngle"].toInt()));
        pThis->setArrowFilled(selfJson["arrowIsFilled"].toBool());
        pThis->setLinePen(QColor(selfJson["color"].toString()));

        QLineF selfLine;
        auto lineDataComponents = selfJson["line"].toString().split(";");
        if (lineDataComponents.size() > 3) {
            selfLine.setP1(QPointF(lineDataComponents[0].toDouble(), lineDataComponents[1].toDouble()));
            selfLine.setP2(QPointF(lineDataComponents[2].toDouble(), lineDataComponents[3].toDouble()));
        }
        pThis->setLine(selfLine);

        pThis->setArrowHeight(selfJson["arrowHeight"].toDouble());

        return res;
    }
};
using ElegantConnection = ConnectionLineBase<ObjectItems::ElegantConnectionLine>;
using StraightConnection = ConnectionLineBase<ObjectItems::ArrowedConnectionLine>;

}  // namespace ObjectItems


