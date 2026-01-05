#pragma once

#include <GraphObject/PluginObjectInterface.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <set>

namespace Graph {

class GObjectConnectionItem;

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

class GObjectItem :
        public ObjectItems::BasicItem,
        public PluginObjectInterface
{
    Q_OBJECT
public:
    explicit GObjectItem(QGraphicsItem* parent = nullptr);
    ~GObjectItem();

    virtual QJsonObject toJson() const;;
    virtual bool fromJson(const QJsonObject& jsonObj);

    /**
     * @brief setVertexNotFound Меняет внешний вид вершины на "не валидный"
     * @note Для отработки случаев, когда не удалось загрузить
     */
    void setItemNotFound();
    bool isItemFound() const;

    void setVertexSizeType(VertexSizeType vst);
    VertexSizeType getVertexSizeType() const;

    void setTitlePosition(VertexTitlePosition vtp);
    VertexTitlePosition getTitlePosition() const;

    void subscribeAsConnectionFrom(GObjectConnectionItem* pLine);
    void unsubscribeConnectionFrom(GObjectConnectionItem* pLine);

    void subscribeAsConnectionTo(GObjectConnectionItem* pLine);
    void unsubscribeConnectionTo(GObjectConnectionItem* pLine);

    bool isLineSubscribed(GObjectConnectionItem* pLine);

signals:
    void sizeChanged(VertexSizeType prevSizeT, VertexSizeType currentSizeT);

private:
    ObjectItems::TextLabel* m_nameItem{nullptr};
    bool m_isItemFound {true};
    bool m_isTextEditedByUser {false};

    QGraphicsPathItem* m_selectionPathItem {nullptr};

    VertexSizeType m_vertexSizeType {VertexSizeType::VST_Medium};
    VertexTitlePosition m_shapeTitlePos {VertexTitlePosition::VTP_Center};

    std::set<GObjectConnectionItem*> m_connectionsFromThis;
    std::set<GObjectConnectionItem*> m_connectionsToThis;

    void updateSelectionPathItem();

private slots:
    void updateConnectionLines();

protected:
    void updateLabelPosition();

    ObjectItems::TextLabel* getLabel() const;

    virtual void processSizeTypeChange(const QRectF& newSize);
};

}
