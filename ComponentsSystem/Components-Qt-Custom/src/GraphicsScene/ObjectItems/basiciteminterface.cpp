#include "basiciteminterface.hpp"

namespace ObjectItems {

objectId_t BasicItemInterface::createSystemId() {
    static objectId_t currentId{-1};
    return --currentId;
}

bool BasicItemInterface::isSystemObject() const { return m_id < 0; }

void BasicItemInterface::setItemId(objectId_t id) { m_id = id; processIdChange(); }

void BasicItemInterface::setSystemId() { setItemId(createSystemId()); }

objectId_t BasicItemInterface::getItemId() const { return m_id; }

void BasicItemInterface::setObjectType(int typ) { m_itemType = typ; processInternalDataChange(); }

int BasicItemInterface::getObjectType() const { return m_itemType; }

QString BasicItemInterface::getSystemName() const { return m_systemName; }

void BasicItemInterface::setDisplayName(const QString &text) { m_displayName = text; processDisplayNameChange(); }

QString BasicItemInterface::getDisplayName() const { return m_displayName; }

void BasicItemInterface::setDescription(const QString &text) { m_description = text; processInternalDataChange(); }

QString BasicItemInterface::getDescription() const { return m_description; }

void BasicItemInterface::setLinePen(const QPen &penC) { m_linePen = penC; processColorChange(); }

QPen BasicItemInterface::getLinePen() const { return m_linePen; }

void BasicItemInterface::setLineHoverPen(const QPen &penC) { m_lineHoverPen = penC; processColorChange(); }

QPen BasicItemInterface::getLineHoverPen() const { return m_lineHoverPen; }

void BasicItemInterface::setLineSelectionPen(const QPen &penC) { m_selectionPen = penC; processColorChange(); }

QPen BasicItemInterface::getLineSelectionPen() const { return m_selectionPen; }

void BasicItemInterface::setBackgroundBrush(const QBrush &brushC) { m_backgroundBrush = brushC; processColorChange(); }

QBrush BasicItemInterface::getBackgroundBrush() const { return m_backgroundBrush; }

void BasicItemInterface::setBackgroundSelectionBrush(const QBrush &brushC) { m_backgroundSelectionBrush = brushC; processColorChange(); }

QBrush BasicItemInterface::getBackgroundSelectionBrush() const { return m_backgroundSelectionBrush; }

void BasicItemInterface::setBackgroundHoverBrush(const QBrush &brushC) { m_backgroundHoverBrush = brushC; processColorChange(); }

QBrush BasicItemInterface::getBackgroundHoverBrush() const { return m_backgroundHoverBrush; }

void BasicItemInterface::setSystemName(const QString &iText) { m_systemName = iText; }

}
