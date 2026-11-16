#include "selectablewidget.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>

namespace WidgetGaleryHelper {

SelectableWidget::SelectableWidget(QWidget* parent) : QWidget(parent) {
    setLayout(new QGridLayout);

    layout()->addWidget(new QWidget);  // TODO: Придумать другой способ

    m_widgetNameLabel = new QLabel(this);
    m_widgetNameLabel->setWordWrap(true);
    m_widgetNameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    layout()->addWidget(m_widgetNameLabel);
    m_widgetNameLabel->setText("Изображение");

    static_cast<QGridLayout*>(layout())->setRowStretch(0, 9);
    static_cast<QGridLayout*>(layout())->setRowStretch(1, 1);
}

void SelectableWidget::setWidget(QWidget* w, const QString& widgetLabel) {
    auto pItem = layout()->itemAt(0);
    pItem->widget()->deleteLater();
    static_cast<QGridLayout*>(layout())->addWidget(w, 0, 0);

    m_widgetNameLabel->setText(widgetLabel);

    m_widget = w;
}

QWidget* SelectableWidget::widget() const {
    return m_widget;
}

void SelectableWidget::setLabel(const QString& labelText) {
    m_widgetNameLabel->setText(labelText);
}

void SelectableWidget::setSelectionColor(const QColor& col) {
    m_selectionColor = col;
}

bool SelectableWidget::isSelected() const {
    return m_isSelected;
}

void SelectableWidget::setSelectedSilent(bool isSelected) {
    if (isSelected != m_isSelected) {
        m_isSelected = isSelected;
        repaint();
    }
}

void SelectableWidget::setSelected(bool isSelected) {
    setSelectedSilent(isSelected);
    emit selectionToggled(isSelected);
}

void SelectableWidget::paintEvent(QPaintEvent* e) {
    QWidget::paintEvent(e);
    if (isSelected()) {
        QPainter p(this);
        p.setPen(QPen(m_selectionColor, 2));
        p.drawRoundedRect(rect(), 6, 6);
    }
}

void SelectableWidget::mouseReleaseEvent(QMouseEvent* e) {
    QWidget::mouseReleaseEvent(e);
    setSelected(!isSelected());
}

}  // namespace WidgetGaleryHelper
