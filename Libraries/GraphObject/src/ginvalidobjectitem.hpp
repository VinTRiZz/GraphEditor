#pragma once

#include "gobjectitem.hpp"

namespace Graph
{

/**
 * @brief The GInvalidObjectItem class  Объект для отображения невалидных объектов
 */
class GInvalidObjectItem : public GObjectItem
{
public:
    GInvalidObjectItem(const QString& pluginName,
                       const QString& objectName,
                       QGraphicsItem* parent = nullptr);

private:
    QGraphicsRectItem* m_pInvalidRect {nullptr};
    QGraphicsSimpleTextItem* m_pInvalidText {nullptr};

protected:
    void processSizeTypeChange(const QRectF& newSize) override;
};

}
