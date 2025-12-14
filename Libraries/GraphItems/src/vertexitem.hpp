#pragma once

#include <GraphObject/GraphObject.h>
#include <GraphObject/PluginObjectInterface.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <set>

namespace Graph {

class VertexConnectionItem;

/**
 * @brief The VertexSizeType enum Условная градация размера вершины
 */
enum VertexSizeType : int {
    VST_UltraSmall,
    VST_Small,
    VST_Medium,
    VST_Big,
    VST_Huge,
};
QRectF toVertexBoundingRect(VertexSizeType vst);

/**
 * @brief The VertexTitlePosition enum Положение текста на вершине
 */
enum VertexTitlePosition : int {
    VTP_Center = 0,
    VTP_Bottom,
    VTP_Top,
    VTP_RightBottom,
};

class VertexItem :
        public ObjectItems::BasicItem,
        public PluginObjectInterface
{
    Q_OBJECT
public:
    explicit VertexItem(QGraphicsItem* parent = nullptr);
    ~VertexItem();

    virtual void fromGObject(const GObject& vert);
    virtual GObject toGObject() const;

    void setVertexSizeType(VertexSizeType vst);
    VertexSizeType getVertexSizeType() const;

    void setTitlePosition(VertexTitlePosition vtp);
    VertexTitlePosition getTitlePosition() const;

    void subscribeAsConnectionFrom(VertexConnectionItem* pLine);
    void unsubscribeConnectionFrom(VertexConnectionItem* pLine);

    void subscribeAsConnectionTo(VertexConnectionItem* pLine);
    void unsubscribeConnectionTo(VertexConnectionItem* pLine);

    bool isLineSubscribed(VertexConnectionItem* pLine);

    VertexConnectionItem* getPendingConnection() const;

signals:
    void sizeChanged(VertexSizeType prevSizeT, VertexSizeType currentSizeT);

private:
    ObjectItems::TextLabel* m_nameItem{nullptr};

    VertexSizeType m_vertexSizeType {VertexSizeType::VST_Medium};
    VertexTitlePosition m_shapeTitlePos {VertexTitlePosition::VTP_Center};

    std::set<VertexConnectionItem*> m_connectionsFromThis;
    std::set<VertexConnectionItem*> m_connectionsToThis;

    VertexConnectionItem* m_pendingConnection {nullptr};

private slots:
    void updateConnectionLines();

protected:
    void updateLabelPosition();

    void setPendingConnection(VertexConnectionItem* pConnection);

    ObjectItems::TextLabel* getLabel() const;

    virtual void processSizeTypeChange(const QRectF& newSize) = 0;
};

}
