#include "grapheditmode.h"

#include <Components/Logger/Logger.h>

#include "submodes/addlinemode.h"
#include "submodes/addobjectmode.h"
#include "submodes/objectmovemode.h"
#include "submodes/propertyeditmode.h"
#include "submodes/removeobjectmode.h"

namespace Graph {

GraphEditMode::GraphEditMode(QWidget* parent) : GraphModeBase(parent) {}

GraphEditMode::~GraphEditMode() {}

void GraphEditMode::init() {
    addSubmode(new AddLineMode(this));
    addSubmode(new AddObjectMode(this));
    addSubmode(new ObjectMoveMode(this));
    addSubmode(new RemoveObjectMode(this));

    // Уникальный режим
    addSubmode(new PropertyEditMode(this));
}

PropertyEditMode* GraphEditMode::getPropertyEditMode() {
    for (auto* pMode : getSubmodes()) {
        if (auto pPropMode = dynamic_cast<PropertyEditMode*>(pMode)) {
            return pPropMode;
        }
    }
    return nullptr;
}

}  // namespace Graph
