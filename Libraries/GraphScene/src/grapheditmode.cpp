#include "grapheditmode.h"

#include <Common/Logging.h>

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
    auto pPropEditMode = static_cast<PropertyEditMode*>(getSubmodes().back());
    connect(pPropEditMode, &PropertyEditMode::openPropertyEditor, this,
            &GraphEditMode::openPropertyEditor);
    connect(pPropEditMode, &PropertyEditMode::closePropertyEditor, this,
            &GraphEditMode::closePropertyEditor);
}

}  // namespace Graph
