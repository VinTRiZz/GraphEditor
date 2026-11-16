#pragma once

#include <QMenu>
#include <QModelIndex>

#include <memory>

class QAbstractItemModel;

namespace QtCustom::Etc {

/**
 * @brief The ModelContextMenu class Builds a hierarchical menu from a tree model
 *
 * Every model item is represented by a QAction. Items with children become
 * submenus, so the whole model structure is mirrored by the menu. The menu
 * tracks model changes (insertions, removals, moves, resets and data changes)
 * and emits clicked() when a leaf action is triggered.
 */
class ModelContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ModelContextMenu(QWidget* parent = nullptr);
    ~ModelContextMenu() override;

    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    /**
     * @brief setRootIndex Restricts the menu to the children of the given index
     * @param index Root of the displayed subtree. It must belong to the current
     *              model; an invalid index displays the whole model.
     * @note The index is tracked persistently and follows row insertions,
     *       removals and moves. If the root item is removed, the menu falls back
     *       to the invalid root and displays the whole model.
     */
    void setRootIndex(const QModelIndex& index);
    QModelIndex rootIndex() const;

    /**
     * @brief setModelColumn Selects the model column used for item data
     * @param column Model column. Negative values are ignored.
     * @note The tree is always traversed through column 0. Changing the column
     *       only changes the data displayed by actions and rebuilds the menu.
     */
    void setModelColumn(int column);
    int modelColumn() const;

    void refresh();

signals:
    /**
     * @brief clicked Emitted when the action of a model item is triggered
     * @param index Index of the triggered item in the source model at modelColumn()
     */
    void clicked(const QModelIndex& index);

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace QtCustom::Etc
