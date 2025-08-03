#ifndef INFORMATIONOBJECTVIEWMX_H
#define INFORMATIONOBJECTVIEWMX_H

#include <ObjectItems/LabelItem.h>

#include <QLabel>

#include "interactiveobjectview.h"

class InformationObjectView : public InteractiveObjectView {
public:
    explicit InformationObjectView(QWidget* parent);

public slots:
    void updateInformation();
    void updateCursorLabel(const QPoint& currentPos);

    void setCurrentToolName(const QString& toolName);

private:
    ObjectViewItems::LabelItem* m_pCursorLabel{
        nullptr};  //! Объект, который показывает координаты пользователю

    QLabel* m_pInformationLabel{nullptr};
    QString m_currentToolName;

protected:
    void mouseMoveEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
};

#endif  // INFORMATIONOBJECTVIEWMX_H
