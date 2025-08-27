#ifndef WIDGETGALERY_H
#define WIDGETGALERY_H

#include <QScrollArea>

class QGridLayout;

class WidgetGalery : public QScrollArea
{
    Q_OBJECT
public:
    explicit WidgetGalery(QWidget *parent = nullptr);

    void setWidgetSize(QSize wSize);

    void addWidget(QWidget* pWidget);
    void removeWidget(QWidget* pWidget);

    bool containWidget(const std::function<bool (QWidget *)> &predicate) const;

private:
    std::list<QWidget*> m_widgets;
    QSize               m_widgetSize {100, 100};
    QGridLayout*        m_layout {nullptr};

protected:
    void updateLayout();

    // TODO: Add processing clicks as selecting widget from list

    void resizeEvent(QResizeEvent* e) override;
};

#endif // WIDGETGALERY_H
