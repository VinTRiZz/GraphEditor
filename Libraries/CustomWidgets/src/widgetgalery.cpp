#include "widgetgalery.h"

#include <QGridLayout>

WidgetGalery::WidgetGalery(QWidget *parent)
    : QScrollArea{parent}
{
    auto centerWidget = new QWidget(this);
    m_layout = new QGridLayout(centerWidget);
    setWidget(centerWidget);
}

void WidgetGalery::setWidgetSize(QSize wSize)
{
    m_widgetSize = wSize;
}

void WidgetGalery::addWidget(QWidget *pWidget)
{
    pWidget->setParent(m_layout->parentWidget());
    m_widgets.push_back(pWidget);
    updateLayout();
}

void WidgetGalery::removeWidget(QWidget *pWidget)
{
    auto targetWidget = std::find(m_widgets.begin(), m_widgets.end(), pWidget);
    if (targetWidget != m_widgets.end()) {
        m_widgets.erase(targetWidget);
        updateLayout();
    }
}

bool WidgetGalery::containWidget(const std::function<bool (QWidget *)> &predicate) const
{
    for (int rw = 0; rw < m_layout->rowCount(); ++rw) {
        for (int col = 0; col < m_layout->columnCount(); ++col) {
            auto pItem = m_layout->itemAtPosition(rw, col);
            if (predicate(pItem->widget())) {
                return true;
            }
        }
    }
    return false;
}

void WidgetGalery::updateLayout()
{
    // TODO: Тут сегфолт
    while (nullptr != m_layout->itemAt(0)) {
        delete m_layout->takeAt(0);
    }

    int currentCol {0};
    int currentRow {0};
    for (auto* pWidget : m_widgets) {
        if ((m_widgetSize.width() + m_layout->margin()) * (currentCol + 1) >= width()) {
            currentCol = 0;
            currentRow++;
        }
        pWidget->setFixedSize(m_widgetSize);
        m_layout->addWidget(pWidget, currentRow, currentCol);
        ++currentCol;
    }
}

void WidgetGalery::resizeEvent(QResizeEvent *e)
{
    updateLayout();
    QScrollArea::resizeEvent(e);
}
