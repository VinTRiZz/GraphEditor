#pragma once

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>

namespace ExtraClasses
{

template <
    typename ValueT,
    typename = std::void_t< // TODO: Workaround this mess
        std::conjunction<
            std::is_default_constructible<ValueT>,
            std::is_copy_constructible<ValueT> > > >
class JOptional
{
    std::shared_ptr<ValueT> m_pValue;
public:
    using type = ValueT;
    JOptional() = default;
    JOptional(const std::nullopt_t&) {} // std::optional workaround
    JOptional(const ValueT& iv) :
        m_pValue {std::make_shared<ValueT>(iv)} {
    }

    // std::optional compatibility
    ValueT& operator*() {
        return *m_pValue;
    }
    const ValueT& operator*() const {
        return *m_pValue;
    }
    ValueT* operator->() {
        return m_pValue.get();
    }
    const ValueT* operator->() const {
        return m_pValue.get();
    }
    bool has_value() const {
        return (nullptr != m_pValue);
    }
    ValueT& value() {
        return *m_pValue;
    }
    const ValueT& value() const {
        return *m_pValue;
    }
    JOptional& operator=(const ValueT& iv) {
        if (!m_pValue) {
            m_pValue = std::make_shared<ValueT>();
        }
        *m_pValue = iv;
        return *this;
    }
    JOptional& operator=([[maybe_unused]] const std::nullopt_t& iv) {
        m_pValue.reset();
        return *this;
    }

    // Json workaround
    JOptional& operator=(const nlohmann::json& iJson) {
        if (iJson.is_null()) {
            m_pValue.reset();
        } else {
            *this = ValueT(iJson);
        }
        return *this;
    }
    operator nlohmann::json() const {
        if (has_value()) {
            return *m_pValue;
        }
        return {};
    }

    operator ValueT() const {
        if (has_value()) {
            return *m_pValue;
        }
        return ValueT{};
    }

    bool operator==(const JOptional& _ov) const {
        if (!has_value() || !_ov.has_value())
            return false;
        return value() == _ov.value();
    }
    bool operator==(const ValueT& _oov) const {
        if (!has_value())
            return false;
        return value() == _oov;
    }
    bool operator<(const JOptional& _ov) const {
        if (!has_value()) {
            return _ov.has_value();
        }
        return value() < _ov.value();
    }
    bool operator<(const ValueT& _oov) const {
        if (!has_value()) {
            return true;
        }
        return value() < _oov;
    }

    // Debug
    friend std::ostream& operator<<(std::ostream& os, const JOptional& jOptional) {
        if (jOptional.has_value()) {
            os << jOptional.value();
        } else {
            os << "NULL";
        }
        return os; // Return the stream to allow chaining
    }
};

}
