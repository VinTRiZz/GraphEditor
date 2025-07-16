#include "vertexobject.h"

#include <QLabel>

#include <QGraphicsSceneMouseEvent>

#include <QTextOption>

#include "logging.h"

#include <QFileInfo>
#include <QImageReader>

#include "Graph/graphcommon.h"

namespace PredefinedObjects
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


VertexObject::VertexObject(QGraphicsItem *parent) :
    QGraphicsRectItem(parent)
{
    setPen(QPen(Qt::black));

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);

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
    m_vertexImage->setZValue(0);

    m_vertexEllipse = new QGraphicsEllipseItem(this);
    m_vertexEllipse->setBrush(GraphCommon::DEFAULT_VERTEX_COLOR);
    m_vertexEllipse->setZValue(0);

    m_vertexTextRect = new QGraphicsRectItem(this);
    m_vertexTextRect->setZValue(0);
    m_vertexTextRect->setBrush(GraphCommon::DEFAULT_VERTEX_TEXT_BGR_COLOR);

    setupTextItem();
}

void VertexObject::setImage(const QImage &img)
{
    m_vertexImage->setPixmap(QPixmap::fromImage(img));
    m_vertexImage->show();

    m_vertexEllipse->hide();
}

void VertexObject::setText(const QString &text)
{
    m_vertexText->setPlainText(text);
}

void VertexObject::setNodeColor(const QColor &borderColor, const QBrush &backgroundBrush)
{
    if (borderColor.isValid()) {
        m_vertexEllipse->setPen(QPen(borderColor, 5));
    } else {
        m_vertexEllipse->setPen(QPen(GraphCommon::DEFAULT_VERTEX_BORDER_COLOR, 5));
    }

    if (backgroundBrush.color().isValid()) {
        m_vertexEllipse->setBrush(backgroundBrush);
    } else {
        m_vertexEllipse->setBrush(GraphCommon::DEFAULT_VERTEX_COLOR);
    }
}

void VertexObject::setVertexBrush(const QBrush &mainBackground, const QBrush &textBackground)
{
    m_vertexEllipse->setBrush(mainBackground);
    m_vertexTextRect->setBrush(textBackground);
}

void VertexObject::setRect(const QRectF &iRect)
{
    // Задаю прямоугольник, чтобы boundingRect() подхватил область определения вершины
    QGraphicsRectItem::setRect(iRect);
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
    m_vertexImage->moveBy((imageRect.width() - m_vertexImage->boundingRect().width()) / 2, 0);

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

void VertexObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void VertexObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    setSelected(!isSelected());

    if (isSelected()) [[unlikely]] {
        m_selectedRectItem->show();
    } else {
        m_selectedRectItem->hide();
    }
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

}
