#include "gconnection.h"

#include <Common/Logging.h>

bool Graph::GConnection::operator==(const GConnection& oCon_) const {
    auto weightEquality =
        (fabs(oCon_.connectionWeight - connectionWeight) < 1e-6);
    auto lineColorsEqual = CommonFunctions::encodeColor(lineColor) ==
                           CommonFunctions::encodeColor(oCon_.lineColor);
    auto mainCompareRes = applyOperator(oCon_, std::equal_to<>{});

    auto res = mainCompareRes && weightEquality && lineColorsEqual;
    if (!res) {
        LOG_INFO("GConnection::equal is false. Comparisons:");
        LOG_INFO("Main compare:", mainCompareRes);
        LOG_INFO("Colors:", lineColorsEqual);
        LOG_INFO("Weight", weightEquality);
    }

    return res;
}

bool Graph::GConnection::operator!=(const GConnection& oCon_) const {
    return !(*this == oCon_);
}
