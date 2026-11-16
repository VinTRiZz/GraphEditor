#pragma once

#include "treeobject.hpp"

#include <algorithm>

namespace ExtraClasses
{

template <typename DataT>
TreeNode<DataT>::TreeNode(const DataT& iData) :
    m_data {iData} {
    setupAsList();
}

template <typename DataT>
TreeNode<DataT>::TreeNode(const ptr_type& pParent) :
    m_parent {pParent} {
    setupAsList();
}

template <typename DataT>
TreeNode<DataT>::~TreeNode()
{
    setParent(nullptr);
}

template<typename DataT>
template<typename... Args>
typename TreeNode<DataT>::ptr_type TreeNode<DataT>::create(Args &&... args) {
    return std::make_shared<item_type>(std::forward<Args>(args)...);
}

template <typename DataT>
template <typename FuncT, typename... Args>
bool TreeNode<DataT>::callRecursive(FuncT&& func, Args&&... args) {
    if (func(this->shared_from_this())) {
        return true;
    }
    return std::visit([&func](auto& cont){
        for (auto& pChild : cont) {
            if (pChild->callRecursive(func)) {
                return true;
            }
        }
        return false;
    }, m_children);
}

template <typename DataT>
template <typename FuncT, typename... Args>
bool TreeNode<DataT>::callRecursive(FuncT&& func, Args&&... args) const {
    if (func(this->shared_from_this())) {
        return true;
    }
    return std::visit([&func](auto& cont){
        for (auto& pChild : cont) {
            if (pChild->callRecursive(func)) {
                return true;
            }
        }
        return false;
    }, m_children);
}

template <typename DataT>
template <typename FuncT, typename... Args>
bool TreeNode<DataT>::callBackwardRecursive(FuncT&& func, Args&&... args) {
    if (func(this->shared_from_this())) {
        return true;
    }
    auto pParent = getParent();
    if (pParent) {
        return pParent->callBackwardRecursive(func, std::forward<Args>(args)...);
    }
    return false;
}

template <typename DataT>
template <typename FuncT, typename... Args>
bool TreeNode<DataT>::callBackwardRecursive(FuncT&& func, Args&&... args) const {
    if (func(this->shared_from_this())) {
        return true;
    }
    auto pParent = getParent();
    if (pParent) {
        return pParent->callBackwardRecursive(func, std::forward<Args>(args)...);
    }
    return false;
}


template <typename DataT>
void TreeNode<DataT>::setParent(const ptr_type& pParent) {
    auto pSelfParent = m_parent.lock();
    if (pSelfParent == pParent) {
        return;
    }
    if (pParent && pSelfParent) {
        pSelfParent->removeNode(this->shared_from_this());
    }
    m_parent = pParent;
    if (pParent) {
        pParent->addNode(this->shared_from_this());
    }
}

template <typename DataT>
void TreeNode<DataT>::setParent(const ptr_type& pParent, std::size_t index) {
    auto pSelfParent = m_parent.lock();
    if (pSelfParent) {
        pSelfParent->removeNode(this->shared_from_this());
    }
    m_parent = pParent;
    if (pParent) {
        if (index > pParent->getNodeCount()) {
            index = pParent->getNodeCount();
        }
        pParent->insertNode(index, this->shared_from_this());
    }
}

template <typename DataT>
typename TreeNode<DataT>::ptr_type TreeNode<DataT>::getParent() const {
    return m_parent.lock();
}



template <typename DataT>
bool TreeNode<DataT>::addNode(std::size_t beforeIndex) {
    return std::visit([beforeIndex, this](auto& cont){
        if (beforeIndex < cont.size() ||
                beforeIndex == 0) {
            auto targetPos = cont.begin();
            std::advance(targetPos, beforeIndex);
            cont.insert(targetPos, std::make_shared<TreeNode<DataT> >(this->shared_from_this()));
            return true;
        }
        return false;
    }, m_children);
}

template <typename DataT>
void TreeNode<DataT>::addNode(const ptr_type& pNode) {
    std::visit([this, pNode](auto& cont){
        cont.push_back(pNode);
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::insertNode(std::size_t index, const ptr_type& pNode) {
    if (!pNode) {
        return false;
    }
    return std::visit([index, pNode](auto& cont){
        if (index > cont.size()) {
            return false;
        }
        auto targetPos = cont.begin();
        std::advance(targetPos, index);
        cont.insert(targetPos, pNode);
        return true;
    }, m_children);
}

template <typename DataT>
void TreeNode<DataT>::addNode(const DataT& nodeData) {
    auto pNode = std::make_shared<TreeNode<DataT> >(this->shared_from_this());
    pNode->setData(nodeData);
    return addNode(pNode);
}

template <typename DataT>
typename TreeNode<DataT>::ptr_type TreeNode<DataT>::takeNode(std::size_t index) {
    return std::visit([index, this](auto& cont) -> ptr_type {
        if (index < cont.size()) {
            auto targetPos = cont.begin();
            std::advance(targetPos, index);
            auto pRes = *targetPos;
            cont.erase(targetPos);
            return pRes;
        }
        return {};
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(ptr_type pNode) {
    return std::visit([pNode, this](auto& cont){
        auto targetPos = std::find(cont.begin(), cont.end(), pNode);
        if (targetPos != cont.end()) {
            cont.erase(targetPos);
            return true;
        }
        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(std::size_t index) {
    return std::visit([index, this](auto& cont){
        if (index < cont.size()) {
            auto targetPos = cont.begin();
            std::advance(targetPos, index);
            cont.erase(targetPos);
            return true;
        }
        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(const DataT &value) {
    return removeNode(findNode(value));
}

template <typename DataT>
void TreeNode<DataT>::clearNodes() {
    return std::visit([this](auto& cont){
        cont.clear();
    }, m_children);
}

template <typename DataT>
typename TreeNode<DataT>::ptr_type TreeNode<DataT>::getNode(std::size_t index) const {
    return std::visit([index, this](auto& cont) -> ptr_type {
        if (index < cont.size()) {
            auto targetPos = cont.begin();
            std::advance(targetPos, index);
            return *targetPos;
        }
        return {};
    }, m_children);
}

template<typename DataT>
typename TreeNode<DataT>::ptr_type TreeNode<DataT>::findNode(const DataT& value) const
{
    return std::visit([&value, this](auto& cont) -> ptr_type {
        auto targetPos = std::find_if(cont.begin(), cont.end(), [&value](auto& pNode){
            return (pNode->getData() == value);
        });
        if (targetPos != cont.end()) {
            return *targetPos;
        }
        return {};
    }, m_children);
}

template<typename DataT>
std::size_t TreeNode<DataT>::getDepth() const
{
    std::size_t depth {0};
    auto pParent = m_parent.lock();
    if (pParent) {
        pParent->callBackwardRecursive([&depth](const auto& pNode) -> bool {
            ++depth;
            return false;
        });
    }
    return depth;
}

template<typename DataT>
std::size_t TreeNode<DataT>::getTotalChildCount() const
{
    std::size_t count {0};
    callRecursive([&count](const auto& pNode) -> bool {
        ++count;
        return false;
    });
    --count; // Remove self from count
    return count;
}

template <typename DataT>
std::size_t TreeNode<DataT>::getNodeCount() const {
    return std::visit([](auto& cont){
        return cont.size();
    }, m_children);
}

template <typename DataT>
std::size_t TreeNode<DataT>::getNodeRow(const ptr_type& pTargetNode) const {
    return std::visit([pTargetNode, this](auto& cont) -> std::size_t {
        std::size_t stepCount {};
        auto targetPos = std::find_if(cont.begin(), cont.end(), [pTargetNode, &stepCount](const auto& pNode){
            if (pTargetNode == pNode) {
                return true;
            }
            ++stepCount;
            return false;
        });
        if (targetPos != cont.end()) {
            return stepCount;
        }
        return {};
    }, m_children);
}

template<typename DataT>
template<typename FuncT>
inline void TreeNode<DataT>::sortNodes(FuncT &&func)
{
    return std::visit([&](auto& cont) {
        using cont_t = std::decay_t<decltype(cont)>;
        if constexpr (std::is_same_v<cont_t, std::vector<ptr_type> >) {
            std::sort(cont.data(), cont.data() + cont.size(), func);
        } else {
            cont.sort(func);
        }
    }, m_children);
}


template <typename DataT>
void TreeNode<DataT>::setData(const DataT& iData) {
    m_data = iData;
}

template <typename DataT>
void TreeNode<DataT>::setData(DataT&& iData) {
    m_data = std::forward<DataT>(iData);
}

template <typename DataT>
const DataT& TreeNode<DataT>::getData() const {
    return m_data;
}

template <typename DataT>
DataT& TreeNode<DataT>::getData() {
    return m_data;
}

template<typename DataT>
void TreeNode<DataT>::setupAsList()
{
    if (std::holds_alternative< std::vector<ptr_type > >(m_children)) {
        auto childrenV = std::get< std::vector<ptr_type > >(m_children);
        std::list<ptr_type > children;
        std::copy(childrenV.begin(), childrenV.end(), std::back_inserter(children));
        m_children = std::move(children);
    }
}

template<typename DataT>
void TreeNode<DataT>::setupAsVector()
{
    if (std::holds_alternative< std::list<ptr_type > >(m_children)) {
        auto childrenL = std::get< std::list<ptr_type > >(m_children);
        std::vector<ptr_type > children;
        children.reserve(childrenL.size());
        std::copy(childrenL.begin(), childrenL.end(), std::back_inserter(children));
        m_children = std::move(children);
    }
}

}
