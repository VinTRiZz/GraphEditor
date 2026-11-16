#pragma once

#include "appsetting.hpp"

#include <iostream>

namespace Common
{

template <typename ValueT>
class NumericSetting : public AppSetting
{
    ValueT m_minV {std::numeric_limits<ValueT>::min()};
    ValueT m_maxV {std::numeric_limits<ValueT>::max()};
public:
    void setMin(ValueT minV) {
        m_minV = minV;
        if (!isSet()) {
            return;
        }
        auto v = getValue<ValueT>();
        if (v < m_minV) {
            setValue(m_minV);
        }
    }
    void setMax(ValueT maxV) {
        m_maxV = maxV;
        if (!isSet()) {
            return;
        }
        auto v = getValue<ValueT>();
        if (v > m_maxV) {
            setValue(m_maxV);
        }
    }

    ValueT getMin() const {
        return m_minV;
    }
    ValueT getMax() const {
        return m_maxV;
    }

    virtual bool setValue(const AppSettingValue_t& v) override {
        if (std::holds_alternative<std::monostate>(v)) {
            return AppSetting::setValue(v);
        }
        if (!std::holds_alternative<ValueT>(v)) {
            return false;
        }
        auto& numV = std::get<ValueT>(v);
        if (numV < m_minV || numV > m_maxV) {
            return false;
        }
        return AppSetting::setValue(numV);
    }
};
using AppIntSetting = NumericSetting<int64_t>;
using AppDoubleSetting = NumericSetting<double>;

}