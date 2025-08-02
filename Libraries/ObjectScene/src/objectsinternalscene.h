#ifndef OBJECTSINTERNALSCENE_H
#define OBJECTSINTERNALSCENE_H

#include <QGraphicsScene>
#include <QHash>

namespace Ui {
class ObjectsInternalScene;
}

class ObjectsInternalScene final : public QGraphicsScene
{
    Q_OBJECT

    // Запреты чтобы не сломать внутреннюю логику сцены
    using QGraphicsScene::addEllipse;
    using QGraphicsScene::addRect;
    using QGraphicsScene::addLine;
    using QGraphicsScene::addPath;
    using QGraphicsScene::addPixmap;
    using QGraphicsScene::addPolygon;
    using QGraphicsScene::addSimpleText;
    using QGraphicsScene::addText;
    using QGraphicsScene::addWidget;

public:
    explicit ObjectsInternalScene(QObject *parent = nullptr);
    ~ObjectsInternalScene();

    void setGridEnabled(bool enabled);
    bool getIsGridEnabled() const;

    void setGridSize(int sizePx);

    using QGraphicsScene::addItem;

protected:
    void drawForeground(QPainter* painter, const QRectF& rect) override;

private:
    bool    m_isGridEnabled = true; //! Вкл/выкл сетки
    double  m_baseGridSize = 50;    //! Размер ячейки при масштабе 1х1

    QPen    m_gridPen {QColor(15, 90, 180, 100), 1}; //! Перо сетки
};

#endif // OBJECTSINTERNALSCENE_H
