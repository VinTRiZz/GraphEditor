#ifndef INFORMATIONOBJECTVIEWMX_H
#define INFORMATIONOBJECTVIEWMX_H

#include "interactiveobjectview.h"

#include <ObjectItems/LabelItem.h>

class InformationObjectView : public InteractiveObjectView
{
public:
    explicit InformationObjectView(QWidget* parent);

private:
    ObjectViewItems::LabelItem* m_pCoordinatesItem  {nullptr};  //! Объект, который показывает координаты пользователю

protected:
    void mouseMoveEvent(QMouseEvent* e) override;
};

#endif // INFORMATIONOBJECTVIEWMX_H
