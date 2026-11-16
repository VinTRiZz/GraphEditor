#pragma once

#include <memory>
#include <atomic>
#include <stdexcept>

namespace ExtraClasses {

/**
 * @brief The Handler class Base for pointer handlers
 */
template <typename ValueT>
class Handler
{
    // Extra metaclasses
    template <typename T, typename = void>
    struct is_lessComparable : std::false_type {};

    template <typename T>
    struct is_lessComparable<T, std::void_t<decltype(std::declval<const T&>() < std::declval<const T&>())>> : std::true_type {};

public:
    using value_t = ValueT;

    Handler() = default;
    Handler(Handler&& _ohdl) = default;
    Handler(const Handler& _ohdl) = default;
    explicit Handler(value_t* pTarget) { setPointer(pTarget); }
    explicit Handler(value_t& pTarget) { setPointer(&pTarget); }
    ~Handler() = default;

    // Analog of std::static_pointer_cast
    template <typename DerivedT>
    DerivedT* cast() noexcept(false) {
        static_assert(std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                      "Derived class must be related to handler type");
        checkValid();
        return static_cast<DerivedT*>(m_pTarget);
    }
    template <typename DerivedT>
    const DerivedT* cast() const noexcept(false) {
        static_assert(std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                      "Derived class must be related to handler type");
        checkValid();
        return static_cast<const DerivedT*>(m_pTarget);
    }

    // Analog of std::dynamic_pointer_cast
    template <typename DerivedT>
    DerivedT* cast_dynamic() noexcept(false) {
        checkValid();
        return dynamic_cast<DerivedT*>(m_pTarget);
    }
    template <typename DerivedT>
    const DerivedT* cast_dynamic() const noexcept(false) {
        checkValid();
        return dynamic_cast<const DerivedT*>(m_pTarget);
    }

    // Pointer access operators
    value_t* operator->() noexcept(false) {
        checkValid();
        return m_pTarget;
    }
    const value_t* operator->() const noexcept(false) {
        checkValid();
        return m_pTarget;
    }

    value_t& operator*() noexcept(false) {
        checkValid();
        return *m_pTarget;
    }
    const value_t& operator*() const noexcept(false) {
        checkValid();
        return *m_pTarget;
    }

    // Copy of STL smart pointers logic
    value_t* get() noexcept { return isValid() ? m_pTarget : nullptr; }
    const value_t* get() const noexcept { return isValid() ? m_pTarget : nullptr; }
    explicit operator bool() const { return isValid(); }

    // Handler source management
    bool isValid() const noexcept { return *m_isValid; }
    void invalidate() { setPointer(nullptr); }

    // For std::set and others using. Default is pointer address sort
    bool operator <(const Handler& _ohdl) const noexcept {
        if (!isValid() || !_ohdl.isValid()) {
            return isValid();
        }
        if constexpr (is_lessComparable<ValueT>::value) {
            return (*m_pTarget < *_ohdl.m_pTarget);
        } else {
            return (m_pTarget < _ohdl.m_pTarget);
        }
    }

    // Comparing
    bool operator ==(value_t* _oval) const noexcept {
        return isValid() && (m_pTarget == _oval);
    }
    bool operator ==(const Handler& _ohdl) const noexcept {
        return isValid() && _ohdl.isValid() && (m_pTarget == _ohdl.m_pTarget);
    }

    // Assigment
    Handler& operator =(const Handler& _ohdl) noexcept = default;
    Handler& operator =(Handler&& _ohdl) noexcept = default;
    Handler& operator =(value_t* _ovalue) noexcept { setPointer(_ovalue); return *this; }

private:
    value_t* m_pTarget {nullptr};
    std::shared_ptr<std::atomic_bool> m_isValid { std::make_shared<std::atomic_bool>(false) };

    void setPointer(value_t* pTarget) {
        *m_isValid = false;
        m_pTarget = pTarget;
        *m_isValid = (nullptr != m_pTarget);
    }

    void checkValid() const noexcept(false) {
        if (!isValid()) {
            throw std::runtime_error("Handler invalidated");
        }
    }
};

} // namespace ExtraClasses

// Hash reimplementation
namespace std {
template <typename T>
struct hash<ExtraClasses::Handler<T> > {
    std::size_t operator()(const ExtraClasses::Handler<T>& hdl) const noexcept {
        return std::hash<const void*>{}(reinterpret_cast<const void*>(hdl.get()));
    }
};
}