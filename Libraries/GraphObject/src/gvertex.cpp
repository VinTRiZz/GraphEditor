#include "gvertex.h"

#include <Components/Common/CommonFunctions.h>
#include <Components/Logger/Logger.h>

bool Graph::GVertex::isDisplayNameValid() const {
    return static_cast<unsigned>(displayName.size()) < GRAPH_MAX_SHORTNAME_SIZE;
}

bool Graph::GVertex::isValid() const {
    return isDisplayNameValid() && (id != 0);
}

bool Graph::GVertex::operator==(const GVertex& oVert_) const {
    auto borderColorCompare =
        (CommonFunctions::encodeColor(lineColor) ==
         CommonFunctions::encodeColor(oVert_.lineColor));
    auto bgrColorCompare =
        (CommonFunctions::encodeColor(backgroundColor) ==
         CommonFunctions::encodeColor(oVert_.backgroundColor));

    auto res = applyOperator(oVert_, std::equal_to<>{}) &&
               (std::fabs(posX - oVert_.posX) <
                std::numeric_limits<double>::epsilon()) &&
               (std::fabs(posY - oVert_.posY) <
                std::numeric_limits<double>::epsilon()) &&
               borderColorCompare && bgrColorCompare;

    //    if (!res) {
    //        LOG_DEBUG_SYNC("EQUAL TIE FIELDS IS FALSE:");
    //        LOG_DEBUG_SYNC("Values mine:", id, shortName, name, description);
    //        LOG_DEBUG_SYNC("Values others:", oVert_.id, oVert_.shortName,
    //                       oVert_.name, oVert_.description);
    //        LOG_DEBUG_SYNC("Position equals:",
    //                       (std::fabs(posX - oVert_.posX) <
    //                        std::numeric_limits<double>::epsilon()),
    //                       (std::fabs(posY - oVert_.posY) <
    //                        std::numeric_limits<double>::epsilon()));
    //        LOG_DEBUG_SYNC("Color equals:", borderColorCompare,
    //        bgrColorCompare); LOG_DEBUG_SYNC(
    //            "Colors BORDER:      ",
    //            CommonFunctions::encodeColor(borderColor),
    //            CommonFunctions::encodeColor(oVert_.borderColor));
    //        LOG_DEBUG_SYNC(
    //            "Colors BACKGROUND:  ",
    //            CommonFunctions::encodeColor(backgroundColor),
    //            CommonFunctions::encodeColor(oVert_.backgroundColor));
    //        LOG_DEBUG_SYNC("TIE FIELDS ===============================");
    //    }
    return res;
}

bool Graph::GVertex::operator!=(const GVertex& oVert_) const {
    return !(*this == oVert_);
}
