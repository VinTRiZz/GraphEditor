#pragma once

#include <memory>
#include <vector>
#include <list>
#include <vector>
#include <variant>
#include <functional>

namespace ExtraClasses
{
template <typename DataT> class TreeNode;
template <typename DataT> using TreeNodePtr = std::shared_ptr<TreeNode<DataT> >;
template <typename DataT> using TreeNodeConstPtr = std::shared_ptr<const TreeNode<DataT> >;
template <typename DataT> using TreeParentNodePtr = std::weak_ptr<TreeNode<DataT> >;

/**
 * @brief The TreeNode class Node of a tree with common features
 * @note Is not safe from recursions
 */
template <typename DataT>
class TreeNode : public std::enable_shared_from_this<TreeNode<DataT> >
{
public:
    // For template creating
    using type = DataT;
    using item_type = TreeNode<DataT>;
    using ptr_type = TreeNodePtr<DataT>;

    explicit TreeNode(const ptr_type& pParent = {});
    explicit TreeNode(const DataT& iData);
    ~TreeNode();

    template <typename...Args>
    static ptr_type create(Args&&... args);

    // Call operation recursively, including this object
    template <typename FuncT, typename... Args>
    bool callRecursive(FuncT&& func, Args&&... args);
    template <typename FuncT, typename... Args>
    bool callRecursive(FuncT&& func, Args&&... args) const;
    template <typename FuncT, typename... Args>
    bool callBackwardRecursive(FuncT&& func, Args&&... args);
    template <typename FuncT, typename... Args>
    bool callBackwardRecursive(FuncT&& func, Args&&... args) const;

    // Working with parent node
    void setParent(const ptr_type& pParent);
    void setParent(const ptr_type& pParent, std::size_t index);
    ptr_type getParent() const;

    // Add a node to the end
    bool addNode(std::size_t beforeIndex);
    void addNode(const ptr_type& pNode);
    void addNode(const DataT& nodeData);

    // Insert an existing node before index
    bool insertNode(std::size_t index, const ptr_type& pNode);

    // Remove node and return it
    ptr_type takeNode(std::size_t index);

    // Remove node if it found
    bool removeNode(ptr_type pNode);
    bool removeNode(std::size_t index);
    bool removeNode(const DataT& value);

    void clearNodes();

    // Utility methods
    ptr_type getNode(std::size_t index) const;
    ptr_type findNode(const DataT &value) const;
    std::size_t getDepth() const;
    std::size_t getTotalChildCount() const;
    std::size_t getNodeCount() const;
    std::size_t getNodeRow(const ptr_type &pNode) const;

    template <typename FuncT>
    void sortNodes(FuncT&& func);

    // Work with data of a node
    void setData(const DataT& iData);
    void setData(DataT&& iData);
    const DataT& getData() const;
    DataT& getData();

    // Changes type of internal container for optimisation cases
    void setupAsList();
    void setupAsVector();
    
private:
    TreeParentNodePtr<DataT> m_parent;
    std::variant< std::list<ptr_type >, std::vector<ptr_type > > m_children;
    DataT m_data {};
};

}

#include "treeobject.inl"
