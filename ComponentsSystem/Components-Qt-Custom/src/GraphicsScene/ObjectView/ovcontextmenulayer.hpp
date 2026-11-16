#pragma once

#include <QMenu>
#include <QGraphicsItem>

#include "ovcontextmenulayer.hpp"

#include "../ObjectItems/basicitem.hpp"

#include <QContextMenuEvent>

#include <QAction>

namespace OVLayers {

template <typename BaseView>
class OVContextMenuLayer
{
private:
    QMenu m_mainContextMenu;  //! Основное контекстное меню

protected:
    void executeContextMenu(QContextMenuEvent* e) {
        m_mainContextMenu.clear();

        QMenu* pSubmenu {nullptr}; // TODO: Придумать, как удалять по-нормальному

        auto pHoverItem = static_cast<BaseView*>(this)->getTopItem(e->pos());

        auto pHoverItemObject = dynamic_cast<ObjectItems::BasicItem*>(pHoverItem);
        if (pHoverItemObject != nullptr) {
            pSubmenu = pHoverItemObject->createContextMenu();
            m_mainContextMenu.addMenu(pSubmenu);
        }

        auto pGridAction = m_mainContextMenu.addAction("Сетка", [this]() {
            static_cast<BaseView*>(this)->getScene()->setGridEnabled(!static_cast<BaseView*>(this)->getScene()->getIsGridEnabled());
        });
        pGridAction->setCheckable(true);
        pGridAction->setChecked(static_cast<BaseView*>(this)->getScene()->getIsGridEnabled());

        m_mainContextMenu.addAction("Вернуться в центр", [this](){
            auto pThis = static_cast<BaseView*>(this);
            pThis->resetScale();
            pThis->centerOn(pThis->getCanvas()->boundingRect().center());
        });

        // Подразумевается, что это меню не было определено
        m_mainContextMenu.exec(e->globalPos());
        delete pSubmenu;
    }
};

} // namespace OVLayers
