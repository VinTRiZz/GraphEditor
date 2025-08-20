#include "centeritem.h"

#include <Common/Logging.h>

namespace ObjectViewItems {

CenterItem::CenterItem(ItemBase *parent) : ItemBase(parent) {

  setSystemId();
  setSystemName("Центр сцены");
  setType(ObjectViewConstants::OBJECTTYPE_CANVAS_CENTER);

  setFlag(ItemClipsToShape, true);

  const double maxX = m_centerSize.width();
  const double maxY = m_centerSize.height();
  const double roundEllipseScale = 0.4;

  const double centerSizeX = 5;
  const double centerSizeY = 5;

  m_pCenterEllipse = new QGraphicsEllipseItem(this);
  registerSubitem(m_pCenterEllipse);
  m_pCenterEllipse->setRect(QRectF(maxX / 2.0 - centerSizeX / 2.0,
                                   maxY / 2.0 - centerSizeY / 2.0, centerSizeX,
                                   centerSizeY));
  m_pCenterEllipse->setBrush(QColor(255, 0, 0, 170));
  m_pCenterEllipse->setFlag(ItemIgnoresTransformations, true);
  m_pCenterEllipse->setZValue(3);

  m_pCenterTargetEllipse = new QGraphicsEllipseItem(this);
  registerSubitem(m_pCenterTargetEllipse);
  m_pCenterTargetEllipse->setFlag(ItemIgnoresTransformations, true);
  m_pCenterTargetEllipse->setRect(QRectF(maxX * (1 - roundEllipseScale) * 0.5,
                                         maxY * (1 - roundEllipseScale) * 0.5,
                                         maxX * roundEllipseScale,
                                         maxY * roundEllipseScale));
  m_pCenterTargetEllipse->setZValue(1);

  m_pCenterVerticalLine = new QGraphicsLineItem(this);
  registerSubitem(m_pCenterVerticalLine);
  m_pCenterVerticalLine->setFlag(ItemIgnoresTransformations, true);
  m_pCenterVerticalLine->setLine(QLineF(maxX / 2.0, 0, maxX / 2.0, maxY));
  m_pCenterVerticalLine->setZValue(2);

  m_pCenterHorizontalLine = new QGraphicsLineItem(this);
  registerSubitem(m_pCenterHorizontalLine);
  m_pCenterHorizontalLine->setFlag(ItemIgnoresTransformations, true);
  m_pCenterHorizontalLine->setLine(QLineF(0, maxY / 2.0, maxX, maxY / 2.0));
  m_pCenterHorizontalLine->setZValue(2);

  setBoundingRect(QRectF(0, 0, maxX, maxY));
}

CenterItem::~CenterItem() {}

QPainterPath CenterItem::shape() const {
  QPainterPath res;
  res.addPath(m_pCenterEllipse->shape());
  res.addPath(m_pCenterTargetEllipse->shape());
  res.addPath(m_pCenterVerticalLine->shape());
  res.addPath(m_pCenterHorizontalLine->shape());
  return res;
}

} // namespace ObjectViewItems
