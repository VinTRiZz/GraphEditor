#pragma once

#include <QString>
#include <QVariant>
#include <QColor>

namespace OVLayers {

const QColor DEFAULT_GRIDCOLOR {QColor(15, 90, 180, 100)};
const QColor DEFAULT_CANVASCOLOR {QColor(220, 220, 220)};
const QColor DEFALT_CANVASBORDERCOLOR {QColor(70, 60, 60)};

enum ItemLayers : int {
    CanvasLayer = -1'000'000,
    // СОГЛАШЕНИЕ: Всё до первого системного уровня -- обычные объекты сцены

    // СОГЛАШЕНИЕ: Все уровни далее -- системные
    SystemComponentsLayerBegin = 1'000'000,
    AnchorItemLayer,

    MarkerItemLayer,
    CursorLabelLayer,
    GrabItemLayer,
};

}
