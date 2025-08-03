#ifndef OBJECTSCENE_H
#define OBJECTSCENE_H

#include "informationobjectview.h"

namespace Ui {
class ObjectScene;
}

class ObjectsInternalScene;
class QMenu;

/**
 * @brief The ObjectScene class Посредник для отображения объектов на
 * QGraphicScene с доп функционалом
 */
class ObjectView : public InformationObjectView {
    Q_OBJECT
public:
    explicit ObjectView(QWidget* parent = nullptr);
    ~ObjectView();
};

#endif  // OBJECTSCENE_H
