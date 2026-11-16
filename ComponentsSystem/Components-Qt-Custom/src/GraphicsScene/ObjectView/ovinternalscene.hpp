#pragma once

#include <QGraphicsScene>

// OV -- сокращение от ObjectsView

namespace OVLayers {

class OVInternalScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit OVInternalScene(QObject *parent = nullptr);
    ~OVInternalScene();

    void setGridEnabled(bool enabled);
    bool getIsGridEnabled() const;

    void setGridSize(int sizePx);
    int getGridSize() const;

    void setGridPen(const QPen& gPen);
    void resetGridPen();

signals:
    void gridSizeChanged(int newGridSize);
    void gridEnabled(bool isGEnabled);

protected:
    void drawForeground(QPainter* painter, const QRectF& rect) override;

private:
    bool m_isGridEnabled{false};  //! Вкл/выкл сетки
    double m_baseGridSize;        //! Размер стороны клетки в сетке при масштабе
    QPen m_gridPen;               //! Перо сетки

    void drawGrid(QPainter* painter, const QRectF& rect);
};


}
