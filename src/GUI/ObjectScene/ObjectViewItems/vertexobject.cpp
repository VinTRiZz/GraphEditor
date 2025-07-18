#include "vertexobject.h"

#include <QLabel>

#include <QGraphicsSceneMouseEvent>

#include <QTextOption>

#include <QFileInfo>
#include <QImageReader>

#include <QBuffer>

#include "vertexconnectionline.h"
#include "../objectsceneconstants.h"

#include "logging.h"

#include "Graph/graphcommon.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems
{

// Загрузка изображения с поддержкой прозрачности
QPixmap loadImageWithAlpha(const QString& path) {
    QImageReader reader(path);
    reader.setAutoTransform(true);  // Автоповорот по EXIF
    reader.setDecideFormatFromContent(true);  // Определение формата по содержимому

    if (reader.supportsAnimation()) {  // Для GIF/APNG
        return QPixmap::fromImage(reader.read());
    }
    return QPixmap(path);  // Для PNG/JPEG/BMP/etc
}

QString rectToString(const QRectF& iRect) {
    QString res;

    res = QString("%0:%1:%2:%3").arg(QString::number(iRect.left()), QString::number(iRect.top()),
                                     QString::number(iRect.width()), QString::number(iRect.height()));

    return res;
}

QRectF rectFromString(const QString& iString) {
    auto valuesSplitted = iString.split(":");
    if (valuesSplitted.count() < 4) {
        LOG_WARNING("Invalid format of rect save:", iString);
        return {};
    }

    QRectF res;
    res.setTopLeft(QPointF(valuesSplitted[0].toDouble(), valuesSplitted[1].toDouble()));
    res.setWidth(valuesSplitted[2].toDouble());
    res.setHeight(valuesSplitted[3].toDouble());
    return res;
}

VertexObject::VertexObject(QGraphicsItem *parent) :
    ItemBase(parent)
{
    setName("Vertex object");

    setType(ObjectViewConstants::OBJECTTYPE_VERTEX);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    m_selectedPen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap);
    QRadialGradient gradient (0, 0, 100);
    gradient.setColorAt(0, QColor("#c5ffb3"));
    gradient.setColorAt(0.5, QColor("#a3ff8a"));
    gradient.setColorAt(1, QColor("#5cff37"));
    m_selectedPen.setBrush(gradient);
    m_selectedRectItem = new QGraphicsPathItem(this);
    m_selectedRectItem->setPen(m_selectedPen);
    m_selectedRectItem->hide();

    m_vertexImage   = new QGraphicsPixmapItem(this);
    m_vertexImage->hide();

    m_vertexEllipse = new QGraphicsEllipseItem(this);
    m_vertexTextRect = new QGraphicsRectItem(this);

    setNodeColor(GraphCommon::DEFAULT_VERTEX_BORDER_COLOR, GraphCommon::DEFAULT_VERTEX_COLOR);
    setTextBackgroundBrush(GraphCommon::DEFAULT_VERTEX_TEXT_BGR_COLOR);

    setupTextItem();
}

VertexObject::~VertexObject()
{
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
        pLine->unregister();
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
        pLine->unregister();
    }
}

void VertexObject::setImage(const QImage &img)
{
    m_vertexImage->setPixmap(QPixmap::fromImage(img));
    m_vertexImage->show();
    m_vertexEllipse->hide();
}

void VertexObject::setShortName(const QString &iText)
{
    m_vertexText->setPlainText(iText);
    ItemBase::setShortName(iText);
    setRect(boundingRect());
}

void VertexObject::setNodeColor(const QColor &borderColor, const QBrush &backgroundBrush)
{
    if (borderColor.isValid()) {
        m_vertexEllipse->setPen(QPen(borderColor, 5));
    } else {
        m_vertexEllipse->setPen(QPen(GraphCommon::DEFAULT_VERTEX_BORDER_COLOR, 5));
    }
    m_mainColor = m_vertexEllipse->pen().color();

    if (backgroundBrush.color().isValid()) {
        m_vertexEllipse->setBrush(backgroundBrush);
    } else {
        m_vertexEllipse->setBrush(GraphCommon::DEFAULT_VERTEX_COLOR);
    }
    m_backgroundColor = m_vertexEllipse->brush().color();
}

void VertexObject::setTextBackgroundBrush(const QBrush &textBackground)
{
    m_vertexTextRect->setBrush(textBackground);
}

QColor VertexObject::getBorderColor() const
{
    return m_vertexEllipse->pen().color();
}

QColor VertexObject::getBackgroundColor() const
{
    return m_vertexEllipse->brush().color();
}

QImage VertexObject::getImage() const
{
    return m_vertexImage->pixmap().toImage();
}

void VertexObject::setRect(const QRectF &iRect)
{
    // Задаю прямоугольник, чтобы boundingRect() подхватил область определения вершины
    setBoundingRect(iRect);
    auto itemRect = iRect;
    itemRect.setX(0); itemRect.setY(0); itemRect.setWidth(iRect.width()); itemRect.setHeight(iRect.height());

    // Область, где определяется вершина
    auto itemRoundRect = itemRect;
    itemRoundRect.moveTo(-5, -5);
    itemRoundRect.setWidth(itemRoundRect.width() + 10);
    itemRoundRect.setHeight(itemRoundRect.height() + 10);

    // Выбор
    QPainterPath path;
    path.addRoundedRect(itemRoundRect, 10, 10);
    m_selectedRectItem->setPath(path);

    const double maxParts   = 5.0;
    const double imageParts = 4.0;
    const double textParts  = maxParts - imageParts - 0.2;
    const double minSize    = 25.0;

    // Изображение
    auto imageRect = itemRect;
    imageRect.setHeight(std::max(itemRect.height() * imageParts / maxParts, minSize * 2));
    m_vertexEllipse->setRect(imageRect);

    if (m_vertexImage->isVisible()) {
        m_vertexImage->setPixmap(m_vertexImage->pixmap().scaled(QSize(imageRect.width(), imageRect.height()), Qt::AspectRatioMode::KeepAspectRatio));
    }
    m_vertexImage->setPos({(boundingRect().width() - m_vertexImage->boundingRect().width()) / 2.0, 0});

    // Текст
    auto textSize = std::max(itemRect.height() * textParts * 0.95 / maxParts, minSize);
    auto textRect = itemRect;
    textRect.setHeight(textSize * 0.8);
    textRect.moveTo(0, imageRect.height() * 1.05);
    m_vertexTextRect->setRect(textRect);
    textRect.setHeight(textSize * 1.05);
    textRect.moveTo(0, imageRect.height() * 0.95);
    m_vertexText->setPos((textRect.width() - m_vertexText->boundingRect().width()) / 2, imageRect.height() * 1.05 - textRect.height() / 5);
}

QPainterPath VertexObject::shape() const
{
    QPainterPath res;

    if (m_vertexImage->isVisible()) {
        res.addPath(m_vertexImage->shape());
    }

    if (m_vertexEllipse->isVisible()) {
        res.addPath(m_vertexEllipse->shape());
    }

    res.addPath(m_vertexTextRect->shape());
    return res;
}

void VertexObject::subscribeAsConnectionFrom(VertexConnectionLine *pLine)
{
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

void VertexObject::unsubscribeConnectionFrom(VertexConnectionLine *pLine)
{
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void VertexObject::subscribeAsConnectionTo(VertexConnectionLine *pLine)
{
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

void VertexObject::unsubscribeConnectionTo(VertexConnectionLine *pLine)
{
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

QVariant VertexObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
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

void VertexObject::setupTextItem()
{
    m_vertexText    = new QGraphicsTextItem(this);
    m_vertexText->setDefaultTextColor(Qt::black);
    m_vertexText->setZValue(0);

    QFont font = m_vertexText->font();
    font.setPixelSize(14);
    font.setStyle(QFont::StyleItalic);
    m_vertexText->setFont(font);
    m_vertexText->setDefaultTextColor(Qt::black);

    QTextDocument* doc = m_vertexText->document();
    doc->setDefaultStyleSheet(QString(
        "body {"
        "   background-color: #FFFF00;"  // Желтый фон
        "   padding: 5px;"
        "   margin: 0px;"
        "   border: 2px solid black;"
        "}"
    ));

    QTextOption option = doc->defaultTextOption();
    option.setAlignment(Qt::AlignCenter);
    doc->setDefaultTextOption(option);
}

void VertexObject::updateConnectionLines()
{
    unsigned connectionNumber {0};
    auto vertexRadius = static_cast<double>(boundingRect().width()) / 2.0;

    for (auto pConFrom : m_connectionsFromThis) {
        auto fromPos = QPointF(x() + vertexRadius,
                               y() + 2 * vertexRadius + pConFrom->getArrowSize());

        pConFrom->setPositionFrom(fromPos);
        connectionNumber++;
    }

    connectionNumber = 0;
    for (auto pConTo : m_connectionsToThis) {
        auto conLine = pConTo->getLine();

        auto isConnectionFromLeft = conLine.x1() < x();
        double connectionOffsetMultiplier = (isConnectionFromLeft ? -1 : 1);

        auto lineOffset = static_cast<double>(connectionNumber) / (static_cast<double>(m_connectionsToThis.size() + 1));
        auto xOffset =
                (isConnectionFromLeft ? 0 : vertexRadius) +
                lineOffset * vertexRadius -
                connectionOffsetMultiplier * pConTo->getArrowSize();

        auto toPos = QPointF(x() + xOffset, y() - pConTo->getArrowSize());

        pConTo->setPositionTo(toPos);
        connectionNumber++;
    }
}

void VertexObject::setCustomProperties(const QJsonObject &props)
{
    if (props.contains(CustomPropertyName::PROPERTY_BOUNDINGRECT)) {
        setBoundingRect(rectFromString(props[CustomPropertyName::PROPERTY_BOUNDINGRECT].toString()));
    }

    ItemBase::setCustomProperties(props);
}

QJsonObject VertexObject::getCustomProperties() const
{
    auto res = ItemBase::getCustomProperties();

    res[CustomPropertyName::PROPERTY_BOUNDINGRECT] = rectToString(boundingRect());

    return res;
}

}
