#ifndef GRAPHEDITVIEW_H
#define GRAPHEDITVIEW_H

#include <Components/CustomQt/ObjectView/InformationLayer.h>

#include <GraphObject/Maintainer.h>

class GraphEditView final :
        public OVLayers::OVInformationLayer,
        public Graph::MaintainerUserDecorator {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);
};

#endif  // GRAPHEDITVIEW_H
