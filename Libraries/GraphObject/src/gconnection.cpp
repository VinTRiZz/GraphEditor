#include "gconnection.h"

#include <Components/Logger/Logger.h>

bool Graph::GConnection::operator==(const GConnection& oCon_) const {
    auto lineColorsEqual = CommonFunctions::encodeColor(color) ==
                           CommonFunctions::encodeColor(oCon_.color);
    auto mainCompareRes = applyOperator(oCon_, std::equal_to<>{});

    auto res = mainCompareRes && lineColorsEqual;
    if (!res) {
        LOG_INFO("GConnection::equal is false. Comparisons:");
        LOG_INFO("Main compare:", mainCompareRes);
        LOG_INFO("Colors:", lineColorsEqual);
    }

    return res;
}

bool Graph::GConnection::operator!=(const GConnection& oCon_) const {
    return !(*this == oCon_);
}
