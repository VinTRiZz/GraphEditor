#pragma once

#include <QAbstractItemModel>

#include <memory>

namespace ExtraClasses
{
template <typename DataT> class TreeNode;
}

namespace QtCustom::Models
{

/**
 * @brief The TreeGroupingProxyModel class  Model, creating tree based on source table rows
 * @note    Can work not only with columns, but with objects. Use qvariant_cast for it and inherit proxy
 *          By default, model builds tree using column data from right to left.
 *          Tree is built using rule one row - one group
 */
class TreeGroupingProxyModel : public QAbstractItemModel
{
    Q_OBJECT

    struct ItemMetadata;
    using Node_t = ExtraClasses::TreeNode<ItemMetadata>;
public:
    explicit TreeGroupingProxyModel(QObject *parent = nullptr);
    ~TreeGroupingProxyModel();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Proxy model equ functional
    void setSourceModel(QAbstractItemModel* pModel);
    QAbstractItemModel* sourceModel() const;
    QModelIndex mapToSource(const QModelIndex& idx) const;
    QModelIndex mapFromSource(const QModelIndex& idx) const;
    void setTreeColumn(int col);
    int treeColumn() const;

    // Grouping functionality
    using GroupKey_t = QVariant;
protected:
    /**
     * @brief getGroup          Define first level of a tree
     * @param sourceModelRow
     * @return
     */
    virtual GroupKey_t  getGroup(int sourceModelRow) const;

    /**
     * @brief getParentGroup    Define every upper than first level of a tree
     * @param groupKey
     * @return                  Must return invalid GroupKey_t if level is highest
     */
    virtual GroupKey_t  getParentGroup(const GroupKey_t& groupKey) const;

    /**
     * @brief getGroupHash  Used in optimisation cases
     * @param groupKey      Key, calculated for first group or upper
     * @return
     * @note                Called many times, so must be as optimised as possible
     */
    virtual uint getGroupHash(const GroupKey_t& groupKey) const;

    // Used to get and set data of a group indexes
    virtual bool        setGroupData(GroupKey_t groupKey, int column, const QVariant& value, int role);
    virtual QVariant    getGroupData(GroupKey_t groupKey, int column, int role) const;

    /**
     * @brief canMergeGroups Used to merge groups. Both arguments are in same level
     * @param lgk
     * @param rgk
     * @return  true if groups can be merged
     */
    virtual bool canMergeGroups(const GroupKey_t& lgk, const GroupKey_t& rgk) const;

    /**
     * @brief resetTree Use in cases, when grouping rules changed radically
     */
    void resetTree();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    // Branch processing
    void addNode(const QModelIndex& idx);
    void updateNode(const QModelIndex& idx);
    void removeNode(const QModelIndex& idx);
    void prune(std::shared_ptr<Node_t> pBranchLeaf);
    std::shared_ptr<Node_t> setupMergableNode(std::vector<GroupKey_t> groupBranch, int sourceRow);
    uint getIndexHash(const QModelIndex& sourceIndex) const;
    void resetTreeNodes(); // internal functionality of resetTree() to use without modelReset()

    // Ordering helpers
    int firstSourceRow(const std::shared_ptr<Node_t>& pNode) const;
    std::size_t childInsertPosition(const std::shared_ptr<Node_t>& pParent, int sourceRow) const;

    // Util
    std::shared_ptr<Node_t> toNode(const QModelIndex& idx) const;
    QModelIndex toModelIndex(const std::shared_ptr<Node_t>& pNode, int column = 0) const;
    std::vector<GroupKey_t> getBranchGroups(GroupKey_t rowNodeGroup) const;
};

}