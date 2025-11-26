#pragma once

#include <Components/CustomQt/ObjectView/InformationLayer.h>

#include <GraphObject/Maintainer.h>

class GraphEditView final :
        public OVLayers::OVInformationLayer,
        public Graph::MaintainerUserDecorator {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};
