#include "dynamicareaitem.h"

#include "logging.h"

namespace ObjectViewItems
{

DynamicAreaItem::DynamicAreaItem(QGraphicsItem* parent) :
    ItemBase(parent)
{
    setName("Dynamic area");

    auto fieldSize = 1024;
    m_fieldRect.setWidth(fieldSize);
    m_fieldRect.setHeight(fieldSize);

    m_hardcodeRect = new QGraphicsRectItem(this);
    m_hardcodeRect->setBrush(QColor(210, 215, 210));
    m_hardcodeRect->setRect(m_fieldRect);

    setFieldRect(-1, -1, 1, 1);
}

void DynamicAreaItem::setRectSize(const QRectF &iRect)
{
    if (iRect.width() == 0 || iRect.height() == 0) {
        throw std::invalid_argument("Rect width or height is zero");
    }
    m_fieldRect = iRect;
}

void DynamicAreaItem::setBrush(const QBrush &iBrush)
{
    m_fieldBrush = iBrush;
}

void DynamicAreaItem::setPen(const QPen &iPen)
{
    m_gridPen = iPen;
}

void DynamicAreaItem::registerItem(ItemBase *pItem)
{
    pItem->setParentItem(this);
    m_registeredItems.push_back(pItem);
}

std::list<ItemBase *> DynamicAreaItem::getRegisteredItems() const
{
    return m_registeredItems;
}

void DynamicAreaItem::clearRegisteredItems()
{
    for (auto pItem : m_registeredItems) {
        delete pItem;
    }
    m_registeredItems.clear();
}

void DynamicAreaItem::removeRegisteredItems(ObjectViewConstants::ObjectType objT)
{
    auto removedBeginIt = std::remove_if(m_registeredItems.begin(), m_registeredItems.end(), [objT](auto* pItem){
        auto res = (pItem->getType() == objT);
        if (res) {
            delete pItem;
        }
        return res;
    });

    auto currentIt = removedBeginIt;
    while (currentIt != m_registeredItems.end()) {
        delete *currentIt;
        *currentIt = nullptr;
        currentIt++;
    }
    m_registeredItems.erase(removedBeginIt, m_registeredItems.end());
}

void DynamicAreaItem::setFieldRect(int minX, int minY, int maxX, int maxY)
{
    m_matrixMin.first  = minX;
    m_matrixMin.second = minY;

    m_matrixMax.first  = maxX;
    m_matrixMax.second = maxY;

    updateFieldRect();
}

void DynamicAreaItem::updateFieldRect()
{

}

void DynamicAreaItem::updateRectSizes()
{
    std::size_t currentCol {};
    std::size_t currentRow {};
    for (auto& col : m_fieldMatrix) {
        currentCol++;
        for (auto& row : col) {
            currentRow++;
            row->setRect(m_fieldRect);
            row->setPos(m_fieldRect.width() * currentCol, m_fieldRect.height() * currentRow);
        }
    }
}

QGraphicsItem *DynamicAreaItem::getFieldByPos(const QPointF &p)
{
    // TODO: Убрать после написания динамического ресайза
    return m_hardcodeRect;



    std::size_t targetPosX = p.x() / m_fieldRect.width();
    std::size_t targetPosY = p.y() / m_fieldRect.height();

    bool sizeChanged = false;
    if (m_matrixMin.first > p.x()) {
        m_matrixMin.first = p.x();
        sizeChanged = true;
    }

    if (m_matrixMin.second > p.y()) {
        m_matrixMin.second = p.y();
        sizeChanged = true;
    }

    if (m_matrixMax.first < p.x()) {
        m_matrixMax.first = p.x();
        sizeChanged = true;
    }

    if (m_matrixMax.second < p.y()) {
        m_matrixMax.second = p.y();
        sizeChanged = true;
    }

    if (sizeChanged) {
        updateFieldRect();
    }

    auto targetColumn = m_fieldMatrix.begin();
    std::advance(targetColumn, targetPosX);

    auto targetRow = targetColumn->begin();
    std::advance(targetRow, targetPosX);

    return *targetRow;
}

}
