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

    /**
     * @brief setLineItem Задать новый айтем соединения. Удаляет предыдущий
     * @param pLine
     */
    void setLineItem(ObjectItems::AbstractConnectionLine* pLine);
    ObjectItems::AbstractConnectionLine* getLineItem() const;

private:
    GObjectItem* m_fromVertex{nullptr};
    GObjectItem* m_toVertex{nullptr};

    bool m_isStraightLine {false};

    ObjectItems::AbstractConnectionLine* m_connectionLine {nullptr};

    void updateLine();
};

}  // namespace ObjectItems

