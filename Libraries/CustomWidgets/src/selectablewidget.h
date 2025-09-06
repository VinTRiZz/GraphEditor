#ifndef SELECTABLEWIDGET_H
#define SELECTABLEWIDGET_H

#include <QWidget>

class QLabel;

namespace WidgetGaleryHelper {

class SelectableWidget : public QWidget {
    Q_OBJECT
public:
    explicit SelectableWidget(QWidget* parent = nullptr);

    void setWidget(QWidget* w, const QString& widgetLabel);
    QWidget* widget() const;

    void setLabel(const QString& labelText);

    void setSelectionColor(const QColor& col);

    bool isSelected() const;

    void setSelectedSilent(bool isSelected);

public slots:
    void setSelected(bool isSelected);

signals:
    void selectionToggled(bool isWidgetSelected);

protected:
    bool m_isSelected{false};
    QColor m_selectionColor{Qt::green};
    QLabel* m_widgetNameLabel;
    QWidget* m_widget{nullptr};  // Для упрощения доступа

    void paintEvent(QPaintEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
};

}  // namespace WidgetGaleryHelper

#endif  // SELECTABLEWIDGET_H
