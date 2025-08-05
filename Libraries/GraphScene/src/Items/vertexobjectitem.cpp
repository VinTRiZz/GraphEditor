#include "vertexobjectitem.h"

#include <Common/Logging.h>
#include <Common/ApplicationSettings.h>
#include <GraphObject/Object.h>
#include <ObjectItems/Constants.h>

#include <QBuffer>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QImageReader>
#include <QLabel>
#include <QTextOption>

#include "connectionlineitem.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems {

// Загрузка изображения с поддержкой прозрачности
QPixmap loadImageWithAlpha(const QString& path) {
    QImageReader reader(path);
    reader.setAutoTransform(true);  // Автоповорот по EXIF
    reader.setDecideFormatFromContent(
        true);  // Определение формата по содержимому

    if (reader.supportsAnimation()) {  // Для GIF/APNG
        return QPixmap::fromImage(reader.read());
    }
    return QPixmap(path);  // Для PNG/JPEG/BMP/etc
}

QString rectToString(const QRectF& iRect) {
    QString res;

    res = QString("%0:%1:%2:%3")
              .arg(QString::number(iRect.left()), QString::number(iRect.top()),
                   QString::number(iRect.width()),
                   QString::number(iRect.height()));

    return res;
}

QRectF rectFromString(const QString& iString) {
    auto valuesSplitted = iString.split(":");
    if (valuesSplitted.count() < 4) {
        LOG_WARNING("Invalid format of rect save:", iString);
        return {};
    }

    QRectF res;
    res.setTopLeft(
        QPointF(valuesSplitted[0].toDouble(), valuesSplitted[1].toDouble()));
    res.setWidth(valuesSplitted[2].toDouble());
    res.setHeight(valuesSplitted[3].toDouble());
    return res;
}

VertexObject::VertexObject(QGraphicsItem* parent) : ItemBase(parent) {
    setSystemName("Вершина");

    setType(ObjectViewConstants::OBJECTTYPE_VERTEX);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    m_selectedRectItem = new QGraphicsPathItem(this);
    registerSubitem(m_selectedRectItem);
    m_selectedRectItem->hide();
    m_selectedRectItem->setZValue(1);

    m_vertexImage = new QGraphicsPixmapItem(this);
    registerSubitem(m_vertexImage);
    m_vertexImage->hide();

    m_vertexEllipse = new QGraphicsEllipseItem(this);
    registerSubitem(m_vertexEllipse);

    auto& appSettings = ApplicationSettings::getInstance();

    VertexObject::setSelectedColor(appSettings.getObjectsConfig().getNodeSelectionColor());
    VertexObject::setSecondColor(appSettings.getObjectsConfig().getNodeSecondColor());
    VertexObject::setMainColor(appSettings.getObjectsConfig().getNodeMainColor());

    m_nameItem = new LabelItem(this);
    registerSubitem(m_nameItem);
    m_nameItem->setSecondColor(appSettings.getObjectsConfig().getLabelBackgroundColor());
    m_nameItem->setMainColor(appSettings.getObjectsConfig().getLabelTextColor());
    m_nameItem->setZValue(0);
}

VertexObject::~VertexObject() {
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
        pLine->unregister();
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
        pLine->unregister();
    }
}

LabelItem* VertexObject::getLabel() const {
    return m_nameItem;
}

void VertexObject::setImage(const QImage& img) {
    if (img.isNull()) {
        return;
    }

    auto newImage = QPixmap::fromImage(img);
    newImage = newImage.scaled(m_vertexEllipse->boundingRect().width(),
                               m_vertexEllipse->boundingRect().height());
    m_vertexImage->setPixmap(newImage);
    m_vertexImage->show();
    m_vertexEllipse->hide();

    // Апдейт области
    setRect(boundingRect());
}

void VertexObject::setShortName(const QString& iText) {
    m_nameItem->setShortName(iText);
    ItemBase::setShortName(iText);

    // Апдейт области
    setRect(boundingRect());
}

void VertexObject::setName(const QString& iText) {
    m_nameItem->setName(iText);
    ItemBase::setName(iText);
}

void VertexObject::setMainColor(const QColor& penColor) {
    if (penColor.isValid()) {
        m_vertexEllipse->setPen(QPen(penColor, 5));
    } else {
        auto& appSettings = ApplicationSettings::getInstance();
        m_vertexEllipse->setPen(
            QPen(appSettings.getObjectsConfig().getNodeMainColor(), 5));
    }
    ItemBase::setMainColor(m_vertexEllipse->pen().color());
}

void VertexObject::setSecondColor(const QColor& penColor) {
    if (penColor.isValid()) {
        m_vertexEllipse->setBrush(penColor);
    } else {
        auto& appSettings = ApplicationSettings::getInstance();
        m_vertexEllipse->setBrush(appSettings.getObjectsConfig().getNodeSecondColor());
    }
    ItemBase::setSecondColor(m_vertexEllipse->brush().color());
}

void VertexObject::setSelectedColor(const QColor& penColor) {
    ItemBase::setSelectedColor(penColor);

    auto selectedPen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap);
    QRadialGradient gradient(0, 0, 100);
    gradient.setColorAt(0, QColor("#c5ffb3"));
    gradient.setColorAt(0.5, QColor("#a3ff8a"));
    gradient.setColorAt(1, getSelectedColor());
    selectedPen.setBrush(gradient);
    m_selectedRectItem->setPen(selectedPen);
}

QImage VertexObject::getImage() const {
    return m_vertexImage->pixmap().toImage();
}

QRectF VertexObject::getImageRect() const {
    return m_vertexEllipse->rect();
}

void VertexObject::setRect(const QRectF& iRect) {
    const double selectionPadding = 20;

    // Задаю прямоугольник, чтобы boundingRect() подхватил область определения
    // вершины
    setBoundingRect(iRect);
    auto itemRect = iRect;
    itemRect.setX(0);
    itemRect.setY(0);
    itemRect.setWidth(iRect.width());
    itemRect.setHeight(iRect.height());

    // Изображение
    auto imageRect = itemRect;
    imageRect.setHeight(imageRect.height() - selectionPadding);
    m_vertexEllipse->setRect(imageRect);

    if (m_vertexImage->isVisible()) {
        m_vertexImage->setPixmap(m_vertexImage->pixmap().scaled(
            QSize(itemRect.width(), itemRect.height()),
            Qt::AspectRatioMode::KeepAspectRatio));
    }
    m_vertexImage->setPos(
        {(boundingRect().width() - m_vertexImage->boundingRect().width()) / 2.0,
         0});

    // Текст
    const double labelPadding = 5.0;  // Отступ для визуального разделения
    m_nameItem->setPos(0, boundingRect().height());

    // Выбор
    auto itemRoundRect = itemRect;
    itemRoundRect.moveTo(-selectionPadding / 2.0, -selectionPadding / 2.0);
    itemRoundRect.setWidth(itemRoundRect.width() + selectionPadding);
    itemRoundRect.setHeight(itemRoundRect.height() + selectionPadding +
                            m_nameItem->boundingRect().height());
    QPainterPath path;
    path.addRoundedRect(itemRoundRect, 10, 10);
    m_selectedRectItem->setPath(path);
}

QPainterPath VertexObject::shape() const {
    QPainterPath res;

    if (m_vertexImage->isVisible()) {
        res.addPath(m_vertexImage->shape());
    }

    if (m_vertexEllipse->isVisible()) {
        res.addPath(m_vertexEllipse->shape());
    }

    res.addPath(m_nameItem->shape());
    return res;
}

bool VertexObject::isLineSubscribed(VertexConnectionLine* pLine) {
    // Нет смысла проверять исходящие, т.к. нельзя регистрировать вершину саму
    // на себя
    for (auto pLineTo : m_connectionsToThis) {
        if (pLineTo->getVertexFrom()->getObjectId() ==
            pLine->getVertexFrom()->getObjectId()) {
            return true;
        }
    }
    return false;
}

void VertexObject::subscribeAsConnectionFrom(VertexConnectionLine* pLine) {
    if (this == pLine->getVertexTo()) {
        return;
    }

    if (nullptr != pLine->getVertexFrom()) {
        pLine->getVertexFrom()->unsubscribeConnectionFrom(pLine);
    }

    pLine->setVertexFrom(this);
    m_connectionsFromThis.emplace(pLine);
    updateConnectionLines();
}

void VertexObject::unsubscribeConnectionFrom(VertexConnectionLine* pLine) {
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void VertexObject::subscribeAsConnectionTo(VertexConnectionLine* pLine) {
    if (this == pLine->getVertexFrom()) {
        return;
    }

    if (nullptr != pLine->getVertexTo()) {
        pLine->getVertexTo()->unsubscribeConnectionTo(pLine);
    }

    pLine->setVertexTo(this);
    m_connectionsToThis.emplace(pLine);
    updateConnectionLines();
}

void VertexObject::unsubscribeConnectionTo(VertexConnectionLine* pLine) {
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

QVariant VertexObject::itemChange(GraphicsItemChange change,
                                  const QVariant& value) {
    if (change == ItemPositionChange) {
        updateConnectionLines();
    }

    if (change == ItemSelectedChange) {
        if (value.toBool()) [[unlikely]] {
            m_selectedRectItem->show();
        } else {
            m_selectedRectItem->hide();
        }
    }

    return ItemBase::itemChange(change, value);
}

void VertexObject::updateConnectionLines() {
    unsigned connectionNumber{0};
    auto vertexRadius = static_cast<double>(boundingRect().width()) / 2.0;

    for (auto pConFrom : m_connectionsFromThis) {
        auto fromPos =
            QPointF(x() + vertexRadius,
                    y() + 2 * vertexRadius + pConFrom->getArrowSize());

        pConFrom->setPositionFrom(fromPos);
        connectionNumber++;
    }

    connectionNumber = 0;
    for (auto pConTo : m_connectionsToThis) {
        auto conLine = pConTo->getLine();

        auto isConnectionFromLeft = conLine.x1() < x();
        double connectionOffsetMultiplier = (isConnectionFromLeft ? -1 : 1);

        auto lineOffset = static_cast<double>(connectionNumber) /
                          (static_cast<double>(m_connectionsToThis.size() + 1));
        auto xOffset = (isConnectionFromLeft ? 0 : vertexRadius) +
                       lineOffset * vertexRadius -
                       connectionOffsetMultiplier * pConTo->getArrowSize();

        auto toPos = QPointF(x() + xOffset, y() - pConTo->getArrowSize());

        pConTo->setPositionTo(toPos);
        connectionNumber++;
    }
}

void VertexObject::setCustomProperties(const QJsonObject& props) {
    if (props.contains(CustomPropertyName::PROPERTY_BOUNDINGRECT)) {
        setBoundingRect(rectFromString(
            props[CustomPropertyName::PROPERTY_BOUNDINGRECT].toString()));
    }

    ItemBase::setCustomProperties(props);
}

QJsonObject VertexObject::getCustomProperties() const {
    auto res = ItemBase::getCustomProperties();

    res[CustomPropertyName::PROPERTY_BOUNDINGRECT] =
        rectToString(boundingRect());

    return res;
}

}  // namespace ObjectViewItems
