#ifndef WIDGETGALERY_H
#define WIDGETGALERY_H

#include <QScrollArea>

class QGridLayout;

class WidgetGalery : public QScrollArea
{
    Q_OBJECT
public:
    explicit WidgetGalery(QWidget *parent = nullptr);

    void init();

    void setWidgetSize(QSize wSize);
    void setSelectionColor(const QColor& col);

    void addWidget(QWidget* pWidget, const QString &widgetLabel);
    void removeWidget(QWidget* pWidget);

    bool containWidget(const std::function<bool (QWidget *)> &predicate) const;
    QWidget* getSelectedWidget() const;

private:
    QWidget*            m_currentSelectedWidget {nullptr};
    std::list<QWidget*> m_widgets;

    QColor m_selectionColor {Qt::green};
    QSize  m_widgetSize     {100, 100};

protected:
    void updateLayout();
};

#endif // WIDGETGALERY_H
