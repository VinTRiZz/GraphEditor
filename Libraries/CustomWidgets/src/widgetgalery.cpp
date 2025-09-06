#include "widgetgalery.h"

#include <QGridLayout>

#include "selectablewidget.h"

using ProxyWidget = WidgetGaleryHelper::SelectableWidget;

WidgetGalery::WidgetGalery(QWidget* parent) : QScrollArea(parent) {}

void WidgetGalery::init() {
    auto pLayout = new QGridLayout;
    pLayout->setVerticalSpacing(5);
    pLayout->setSpacing(3);
    QScrollArea::setWidget(new QWidget);
    widget()->setLayout(pLayout);
}

void WidgetGalery::setWidgetSize(QSize wSize) {
    m_widgetSize = wSize;
}

void WidgetGalery::setSelectionColor(const QColor& col) {
    m_selectionColor = col;
    for (auto* pWidget : m_widgets) {
        static_cast<ProxyWidget*>(pWidget)->setSelectionColor(col);
    }
}

void WidgetGalery::addWidget(QWidget* pWidget, const QString& widgetLabel) {
    auto selectableWidget = new ProxyWidget(widget());
    selectableWidget->setWidget(pWidget, widgetLabel);
    selectableWidget->setSelectionColor(m_selectionColor);

    connect(selectableWidget, &ProxyWidget::selectionToggled, selectableWidget,
            [this, selectableWidget](bool isSelectedState) {
                if (!isSelectedState) {
                    m_currentSelectedWidget = nullptr;
                    return;
                }
                m_currentSelectedWidget = selectableWidget;

                for (auto* pWidget : m_widgets) {
                    if (pWidget == selectableWidget) {
                        continue;
                    }
                    static_cast<ProxyWidget*>(pWidget)->setSelected(false);
                }
            });

    pWidget->setParent(selectableWidget);
    m_widgets.push_back(selectableWidget);

    updateLayout();
}

void WidgetGalery::setLabel(QWidget *pWidget, const QString &widgetLabel)
{
    auto targetWidget = std::find_if(
        m_widgets.begin(), m_widgets.end(), [pWidget](auto* pContainerWidget) {
            return (pWidget ==
                    static_cast<ProxyWidget*>(pContainerWidget)->widget());
        });
    if (targetWidget != m_widgets.end()) {
        auto targetContainer = *targetWidget;
        static_cast<ProxyWidget*>(targetContainer)->setLabel(widgetLabel);
    }
}

void WidgetGalery::removeWidget(QWidget* pWidget) {
    auto targetWidget = std::find_if(
        m_widgets.begin(), m_widgets.end(), [pWidget](auto* pContainerWidget) {
            return (pWidget ==
                    static_cast<ProxyWidget*>(pContainerWidget)->widget());
        });
    if (targetWidget != m_widgets.end()) {
        m_widgets.erase(targetWidget);
        updateLayout();
    }
}

bool WidgetGalery::containWidget(
    const std::function<bool(QWidget*)>& predicate) const {
    for (auto* pWidget : m_widgets) {
        if (predicate(static_cast<ProxyWidget*>(pWidget)->widget())) {
            return true;
        }
    }
    return false;
}

QWidget *WidgetGalery::getWidget(const std::function<bool (QWidget *)> &predicate) const
{
    for (auto* pWidget : m_widgets) {
        auto castedWidget = static_cast<ProxyWidget*>(pWidget);
        if (predicate(castedWidget->widget())) {
            return castedWidget->widget();
        }
    }
    return nullptr;
}

QWidget* WidgetGalery::getSelectedWidget() const {
    return m_currentSelectedWidget;
}

void WidgetGalery::updateLayout() {
    auto pCenterWidgetLayout = static_cast<QGridLayout*>(widget()->layout());
    auto pItem = pCenterWidgetLayout->takeAt(0);
    while (nullptr != pItem) {
        delete pItem;
        pItem = pCenterWidgetLayout->takeAt(0);
    }

    int currentCol{0};
    int currentRow{0};
    for (auto* pWidget : m_widgets) {
        if ((m_widgetSize.width() + pCenterWidgetLayout->margin()) *
                (currentCol + 1) >=
            width()) {
            currentCol = 0;
            currentRow++;
        }

        pWidget->setFixedSize(m_widgetSize);
        pCenterWidgetLayout->addWidget(pWidget, currentRow, currentCol);
        ++currentCol;
    }
}
