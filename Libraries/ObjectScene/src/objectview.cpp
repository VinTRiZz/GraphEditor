#include "objectview.h"

#include <QScrollBar>

ObjectView::ObjectView(QWidget* parent) : InformationObjectView(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
}

ObjectView::~ObjectView() {}
