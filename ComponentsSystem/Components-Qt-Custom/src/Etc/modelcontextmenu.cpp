#include "modelcontextmenu.hpp"

#include <QAbstractItemModel>
#include <QAction>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHash>
#include <QIcon>
#include <QImage>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QPainter>
#include <QPersistentModelIndex>
#include <QPixmap>
#include <QPointer>
#include <QProxyStyle>
#include <QStyleOptionMenuItem>
#include <QVariant>
#include <QVector>

namespace QtCustom::Etc {

namespace {

constexpr auto kForegroundProperty = "_qtcustom_model_foreground";
constexpr auto kBackgroundProperty = "_qtcustom_model_background";

QIcon iconFromDecoration(const QVariant& decoration)
{
    if (!decoration.isValid()) {
        return {};
    }

    switch (decoration.userType()) {
    case QMetaType::QIcon:
        return decoration.value<QIcon>();
    case QMetaType::QPixmap:
        return QIcon(decoration.value<QPixmap>());
    case QMetaType::QImage:
        return QIcon(QPixmap::fromImage(decoration.value<QImage>()));
    case QMetaType::QColor: {
        QPixmap pixmap(16, 16);
        pixmap.fill(decoration.value<QColor>());
        return QIcon(pixmap);
    }
    default:
        break;
    }

    if (decoration.canConvert<QIcon>()) {
        return decoration.value<QIcon>();
    }
    return {};
}

QBrush toBrush(const QVariant& value)
{
    if (!value.isValid()) {
        return {};
    }
    if (value.userType() == QMetaType::QColor) {
        return QBrush(value.value<QColor>());
    }
    if (value.canConvert<QBrush>()) {
        return value.value<QBrush>();
    }
    return {};
}

QString actionText(const QVariant& value)
{
    QString text = value.toString();
    text.replace(QLatin1String("&"), QLatin1String("&&"));
    return text;
}

void applyActionPalette(QStyleOptionMenuItem* option, const QAction* action)
{
    if (!option || !action) {
        return;
    }

    const QBrush foreground = toBrush(action->property(kForegroundProperty));
    if (foreground.style() != Qt::NoBrush) {
        option->palette.setBrush(QPalette::Active, QPalette::Text, foreground);
        option->palette.setBrush(QPalette::Inactive, QPalette::Text, foreground);
        option->palette.setBrush(QPalette::Active, QPalette::ButtonText, foreground);
        option->palette.setBrush(QPalette::Inactive, QPalette::ButtonText, foreground);
    }

    const QBrush background = toBrush(action->property(kBackgroundProperty));
    if (background.style() != Qt::NoBrush) {
        option->palette.setBrush(QPalette::Active, QPalette::Button, background);
        option->palette.setBrush(QPalette::Inactive, QPalette::Button, background);
        option->palette.setBrush(QPalette::Active, QPalette::Window, background);
        option->palette.setBrush(QPalette::Inactive, QPalette::Window, background);
        option->palette.setBrush(QPalette::Active, QPalette::Base, background);
        option->palette.setBrush(QPalette::Inactive, QPalette::Base, background);
    }
}

bool hasCustomColors(const QAction* action)
{
    return action->property(kForegroundProperty).isValid()
           || action->property(kBackgroundProperty).isValid();
}

class ModelMenuStyle : public QProxyStyle
{
public:
    explicit ModelMenuStyle(QStyle* baseStyle = nullptr)
        : QProxyStyle(baseStyle)
    {
    }

    void drawControl(ControlElement element, const QStyleOption* option,
                     QPainter* painter, const QWidget* widget = nullptr) const override
    {
        if (element == QStyle::CE_MenuItem && painter) {
            const auto* menu = qobject_cast<const QMenu*>(widget);
            const auto* menuOption =
                qstyleoption_cast<const QStyleOptionMenuItem*>(option);
            if (menu && menuOption) {
                QAction* action = menu->actionAt(menuOption->rect.center());
                if (action && hasCustomColors(action)) {
                    const QBrush background =
                        toBrush(action->property(kBackgroundProperty));
                    if (background.style() != Qt::NoBrush) {
                        painter->fillRect(menuOption->rect, background);
                    }

                    QStyleOptionMenuItem styledOption = *menuOption;
                    applyActionPalette(&styledOption, action);
                    QProxyStyle::drawControl(element, &styledOption, painter, widget);
                    return;
                }
            }
        }

        QProxyStyle::drawControl(element, option, painter, widget);
    }
};

class ModelSubMenu : public QMenu
{
public:
    explicit ModelSubMenu(QWidget* parent = nullptr)
        : QMenu(parent)
    {
    }
};

} // namespace

struct ModelContextMenu::Impl
{
    struct Entry
    {
        QPersistentModelIndex index;
        QAction* action {nullptr};
        QMenu* menu {nullptr};
        QMenu* parentMenu {nullptr};
    };

    ModelContextMenu* q {nullptr};
    ModelMenuStyle* menuStyle {nullptr};
    QPointer<QAbstractItemModel> model;
    QPersistentModelIndex root;
    int column {0};
    bool rootExplicit {false};
    QHash<QPersistentModelIndex, Entry> entries;
    QVector<QMetaObject::Connection> connections;

    ~Impl()
    {
        disconnectModel();
        clearEntries();
    }

    void setModel(QAbstractItemModel* newModel)
    {
        if (model.data() == newModel) {
            return;
        }

        disconnectModel();
        model = newModel;
        rootExplicit = false;
        root = QPersistentModelIndex();

        if (model) {
            connectModel();
        }
        rebuild();
    }

    void setRootIndex(const QModelIndex& index)
    {
        if (index.isValid() && (!model || index.model() != model.data())) {
            return;
        }

        rootExplicit = index.isValid();
        root = index.isValid() ? QPersistentModelIndex(structureIndex(index))
                               : QPersistentModelIndex();
        rebuild();
    }

    void setModelColumn(int value)
    {
        if (value < 0 || column == value) {
            return;
        }
        column = value;
        rebuild();
    }

    QModelIndex structureIndex(const QModelIndex& index) const
    {
        if (!index.isValid() || !model) {
            return index;
        }
        return model->index(index.row(), 0, index.parent());
    }

    QModelIndex displayIndex(const QModelIndex& index) const
    {
        if (!index.isValid() || !model || 0 == column) {
            return index;
        }
        const QModelIndex display = model->index(index.row(), column, index.parent());
        return display.isValid() ? display : index;
    }

    void rebuild()
    {
        clearEntries();
        if (!model) {
            return;
        }

        QModelIndex rootIndex;
        if (rootExplicit) {
            if (root.isValid() && root.model() == model.data()) {
                rootIndex = root;
            } else {
                rootExplicit = false;
                root = QPersistentModelIndex();
            }
        }

        populate(q, rootIndex);
    }

    void connectModel()
    {
        QAbstractItemModel* source = model.data();
        if (!source) {
            return;
        }

        connections.append(QObject::connect(
            source, &QAbstractItemModel::modelAboutToBeReset, q,
            [this]() { clearEntries(); }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::modelReset, q,
            [this]() { rebuild(); }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::layoutChanged, q,
            [this]() { rebuild(); }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::rowsInserted, q,
            [this](const QModelIndex& parent, int first, int last) {
                onRowsInserted(parent, first, last);
            }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::rowsAboutToBeRemoved, q,
            [this](const QModelIndex& parent, int first, int last) {
                onRowsAboutToBeRemoved(parent, first, last);
            }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::rowsRemoved, q,
            [this](const QModelIndex& parent, int, int) {
                onRowsRemoved(parent);
            }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::rowsMoved, q,
            [this](const QModelIndex&, int, int, const QModelIndex&, int) {
                rebuild();
            }));

        connections.append(QObject::connect(
            source, &QAbstractItemModel::dataChanged, q,
            [this](const QModelIndex& topLeft, const QModelIndex& bottomRight,
                   const QVector<int>&) {
                onDataChanged(topLeft, bottomRight);
            }));

        connections.append(QObject::connect(
            source, &QObject::destroyed, q, [this]() {
                model = nullptr;
                rootExplicit = false;
                root = QPersistentModelIndex();
                connections.clear();
                clearEntries();
            }));
    }

    void disconnectModel()
    {
        for (const auto& connection : connections) {
            QObject::disconnect(connection);
        }
        connections.clear();
    }

    void clearEntries()
    {
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if (it->parentMenu && it->action) {
                it->parentMenu->removeAction(it->action);
            }
        }

        QVector<QMenu*> menus;
        QVector<QAction*> actions;
        menus.reserve(entries.size());
        actions.reserve(entries.size());

        for (const auto& entry : entries) {
            if (entry.menu) {
                menus.append(entry.menu);
            } else if (entry.action) {
                actions.append(entry.action);
            }
        }

        entries.clear();

        for (auto* menu : menus) {
            delete menu;
        }
        for (auto* action : actions) {
            delete action;
        }
    }

    void populate(QMenu* target, const QModelIndex& parent)
    {
        if (!target || !model) {
            return;
        }

        const int rows = model->rowCount(parent);
        for (int row = 0; row < rows; ++row) {
            const QModelIndex structure = model->index(row, 0, parent);
            if (structure.isValid()) {
                insertItem(target, structure, displayIndex(structure), nullptr);
            }
        }
    }

    QAction* insertItem(QMenu* target, const QModelIndex& structure,
                        const QModelIndex& display, QAction* beforeAction)
    {
        if (!target || !model || !structure.isValid()) {
            return nullptr;
        }

        const bool isSubmenu = model->hasChildren(structure);
        if (isSubmenu) {
            auto* submenu = new ModelSubMenu(q);
            submenu->setStyle(menuStyle);
            target->insertMenu(beforeAction, submenu);

            const QPersistentModelIndex persistentIndex(structure);
            Entry entry;
            entry.index = persistentIndex;
            entry.action = submenu->menuAction();
            entry.menu = submenu;
            entry.parentMenu = target;
            entries.insert(persistentIndex, entry);

            applyActionData(entry.action, display, true);
            populate(submenu, structure);
            return entry.action;
        }

        auto* action = new QAction(q);
        target->insertAction(beforeAction, action);

        const QPersistentModelIndex persistentIndex(structure);
        Entry entry;
        entry.index = persistentIndex;
        entry.action = action;
        entry.parentMenu = target;
        entries.insert(persistentIndex, entry);

        applyActionData(action, display, false);
        QObject::connect(action, &QAction::triggered, q, [this, persistentIndex]() {
            emit q->clicked(displayIndex(persistentIndex));
        });
        return action;
    }

    void applyActionData(QAction* action, const QModelIndex& index, bool isSubmenu) const
    {
        if (!action || !index.isValid()) {
            return;
        }

        action->setText(actionText(index.data(Qt::DisplayRole)));
        action->setIcon(iconFromDecoration(index.data(Qt::DecorationRole)));

        const Qt::ItemFlags flags = index.flags();
        action->setEnabled(flags.testFlag(Qt::ItemIsEnabled));

        if (isSubmenu) {
            action->setCheckable(false);
            action->setChecked(false);
        } else {
            const QVariant checkState = index.data(Qt::CheckStateRole);
            const bool isCheckable = checkState.isValid()
                                     || flags.testFlag(Qt::ItemIsUserCheckable);
            action->setCheckable(isCheckable);
            action->setChecked(checkState.isValid()
                               && checkState.toInt() != Qt::Unchecked);
        }

        const QVariant toolTip = index.data(Qt::ToolTipRole);
        action->setToolTip(toolTip.isValid() ? toolTip.toString() : QString());
        const QVariant statusTip = index.data(Qt::StatusTipRole);
        action->setStatusTip(statusTip.isValid() ? statusTip.toString() : QString());
        const QVariant whatsThis = index.data(Qt::WhatsThisRole);
        action->setWhatsThis(whatsThis.isValid() ? whatsThis.toString() : QString());

        const QVariant font = index.data(Qt::FontRole);
        action->setFont(font.canConvert<QFont>() ? font.value<QFont>() : QFont());

        action->setProperty(kForegroundProperty, index.data(Qt::ForegroundRole));
        action->setProperty(kBackgroundProperty, index.data(Qt::BackgroundRole));
    }

    bool convertLeafToMenu(const QPersistentModelIndex& index)
    {
        auto it = entries.find(index);
        if (it == entries.end() || it->menu) {
            return false;
        }

        QAction* oldAction = it->action;
        QMenu* parentMenu = it->parentMenu;
        if (!oldAction || !parentMenu) {
            return false;
        }

        const QList<QAction*> oldActions = parentMenu->actions();
        const int position = oldActions.indexOf(oldAction);
        parentMenu->removeAction(oldAction);
        delete oldAction;

        QAction* beforeAction = parentMenu->actions().value(position, nullptr);
        auto* submenu = new ModelSubMenu(q);
        submenu->setStyle(menuStyle);
        parentMenu->insertMenu(beforeAction, submenu);

        it = entries.find(index);
        if (it == entries.end()) {
            delete submenu;
            return false;
        }

        it->action = submenu->menuAction();
        it->menu = submenu;
        applyActionData(it->action, displayIndex(index), true);
        populate(submenu, index);
        return true;
    }

    bool convertMenuToLeaf(const QPersistentModelIndex& index)
    {
        auto it = entries.find(index);
        if (it == entries.end() || !it->menu) {
            return false;
        }

        QMenu* oldMenu = it->menu;
        QMenu* parentMenu = it->parentMenu;
        QAction* oldAction = it->action;
        if (!oldMenu || !parentMenu || !oldAction) {
            return false;
        }

        const QList<QAction*> oldActions = parentMenu->actions();
        const int position = oldActions.indexOf(oldAction);
        QAction* beforeAction = (position >= 0 && position + 1 < oldActions.size())
                                    ? oldActions.at(position + 1)
                                    : nullptr;
        parentMenu->removeAction(oldAction);
        delete oldMenu;

        auto* action = new QAction(q);
        parentMenu->insertAction(beforeAction, action);

        it = entries.find(index);
        if (it == entries.end()) {
            delete action;
            return false;
        }

        it->action = action;
        it->menu = nullptr;
        applyActionData(action, displayIndex(index), false);
        QObject::connect(action, &QAction::triggered, q, [this, index]() {
            emit q->clicked(displayIndex(index));
        });
        return true;
    }

    void removeSubtree(const QPersistentModelIndex& index)
    {
        auto it = entries.find(index);
        if (it == entries.end()) {
            return;
        }

        if (it->menu && model) {
            const QModelIndex modelIndex = index;
            if (modelIndex.isValid()) {
                const int rows = model->rowCount(modelIndex);
                for (int row = 0; row < rows; ++row) {
                    const QModelIndex child = model->index(row, 0, modelIndex);
                    if (child.isValid()) {
                        removeSubtree(QPersistentModelIndex(child));
                    }
                }
            }
        }

        it = entries.find(index);
        if (it == entries.end()) {
            return;
        }

        if (it->parentMenu && it->action) {
            it->parentMenu->removeAction(it->action);
        }

        if (it->menu) {
            delete it->menu;
        } else if (it->action) {
            delete it->action;
        }
        entries.erase(it);
    }

    void onRowsInserted(const QModelIndex& parent, int first, int last)
    {
        if (!model || first < 0 || last < first) {
            return;
        }

        const QModelIndex parentStructure = parent.isValid()
                                                ? structureIndex(parent)
                                                : QModelIndex();
        QMenu* target = q;
        if (parentStructure.isValid() && !(rootExplicit && root == parentStructure)) {
            const QPersistentModelIndex parentIndex(parentStructure);
            auto it = entries.find(parentIndex);
            if (it == entries.end()) {
                return;
            }
            if (!it->menu) {
                convertLeafToMenu(parentIndex);
                return;
            }
            target = it->menu;
        }

        const QList<QAction*> oldActions = target->actions();
        QAction* beforeAction = oldActions.value(first, nullptr);

        for (int row = first; row <= last; ++row) {
            const QModelIndex structure = model->index(row, 0, parentStructure);
            if (structure.isValid()) {
                insertItem(target, structure, displayIndex(structure), beforeAction);
            }
        }
    }

    void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
    {
        if (!model || first < 0 || last < first) {
            return;
        }

        const QModelIndex parentStructure = parent.isValid()
                                                ? structureIndex(parent)
                                                : QModelIndex();
        bool knownParent = false;
        if (!parentStructure.isValid() || (rootExplicit && root == parentStructure)) {
            knownParent = true;
        } else {
            const auto it = entries.constFind(QPersistentModelIndex(parentStructure));
            knownParent = (it != entries.constEnd()) && (it->menu != nullptr);
        }

        if (!knownParent) {
            return;
        }

        for (int row = first; row <= last; ++row) {
            const QModelIndex structure = model->index(row, 0, parentStructure);
            if (structure.isValid()) {
                removeSubtree(QPersistentModelIndex(structure));
            }
        }
    }

    void onRowsRemoved(const QModelIndex& parent)
    {
        if (!model) {
            return;
        }

        if (rootExplicit && !root.isValid()) {
            rootExplicit = false;
            rebuild();
            return;
        }

        const QModelIndex parentStructure = parent.isValid()
                                                ? structureIndex(parent)
                                                : QModelIndex();
        if (!parentStructure.isValid() || (rootExplicit && root == parentStructure)) {
            return;
        }

        const QPersistentModelIndex parentIndex(parentStructure);
        auto it = entries.find(parentIndex);
        if (it == entries.end() || !it->menu) {
            return;
        }

        if (it->menu->actions().isEmpty() && !model->hasChildren(parentStructure)) {
            convertMenuToLeaf(parentIndex);
        }
    }

    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
    {
        if (!model || !topLeft.isValid() || !bottomRight.isValid()) {
            return;
        }
        if (column < topLeft.column() || column > bottomRight.column()) {
            return;
        }

        const QModelIndex parent = topLeft.parent();
        for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
            const QModelIndex structure = model->index(row, 0, parent);
            if (!structure.isValid()) {
                continue;
            }

            const auto it = entries.constFind(QPersistentModelIndex(structure));
            if (it == entries.constEnd()) {
                continue;
            }
            applyActionData(it->action, displayIndex(structure), it->menu != nullptr);
            if (it->parentMenu) {
                it->parentMenu->update();
            }
        }
    }
};

ModelContextMenu::ModelContextMenu(QWidget* parent)
    : QMenu(parent)
    , d {new Impl}
{
    d->q = this;
    d->menuStyle = new ModelMenuStyle(style());
    d->menuStyle->setParent(this);
    setStyle(d->menuStyle);
}

ModelContextMenu::~ModelContextMenu() = default;

void ModelContextMenu::setModel(QAbstractItemModel* model)
{
    d->setModel(model);
}

QAbstractItemModel* ModelContextMenu::model() const
{
    return d->model.data();
}

void ModelContextMenu::setRootIndex(const QModelIndex& index)
{
    d->setRootIndex(index);
}

QModelIndex ModelContextMenu::rootIndex() const
{
    return d->root;
}

void ModelContextMenu::setModelColumn(int column)
{
    d->setModelColumn(column);
}

int ModelContextMenu::modelColumn() const
{
    return d->column;
}

void ModelContextMenu::refresh()
{
    d->rebuild();
}

} // namespace QtCustom::Etc
