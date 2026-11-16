#pragma once

#include <type_traits>
#include <utility>

namespace ExtraClasses {

/**
 * @brief The CommitableObject class Main idea is to make sure object value can be reverted
 */
template <typename ValueT>
class CommitableObject
{
    // Extra metaclasses
    template <typename T, typename = void>
    struct is_compareAvailable : std::false_type {};

    template <typename T>
    struct is_compareAvailable<T, std::void_t<decltype(std::declval<T>().operator<(T{}))>> : std::true_type {};

public:
    ValueT& source() {
        return m_value;
    }
    const ValueT& source() const {
        return m_value;
    }

    ValueT& value() {
        return m_editedValue;
    }
    const ValueT& value() const {
        return m_editedValue;
    }

    // Standard pointer-like actions
    ValueT* operator->() {
        return &m_editedValue;
    }
    const ValueT* operator->() const {
        return &m_editedValue;
    }

    ValueT& operator*() {
        return m_editedValue;
    }
    const ValueT& operator*() const {
        return m_editedValue;
    }

    operator ValueT() const {
        return m_editedValue;
    }

    // If available
    std::enable_if_t<std::is_copy_assignable_v<ValueT>, CommitableObject<ValueT>&>
    operator=(const ValueT& _ov) {
        m_editedValue = _ov;
        return *this;
    }
    std::enable_if_t<std::is_move_assignable_v<ValueT>, CommitableObject<ValueT>&>
    operator=(ValueT&& _ov) {
        m_editedValue = std::move(_ov);
        return *this;
    }

    // If available
    template <typename vT = ValueT, typename = std::void_t<is_compareAvailable<ValueT> > >
    bool operator<(const CommitableObject<ValueT>& _ohdl) const {
        return m_editedValue < _ohdl.m_editedValue;
    }

    void commit() {
        m_value = m_editedValue;
    }
    void revert() {
        m_editedValue = m_value;
    }

private:
    ValueT m_editedValue;
    ValueT m_value;
};

} // namespace Web
