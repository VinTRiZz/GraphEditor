#include "gvertex.h"

#include <Common/Logging.h>

bool Graph::GVertex::isShortnameValid() const {
    return static_cast<unsigned>(shortName.size()) < GRAPH_MAX_SHORTNAME_SIZE;
}

bool Graph::GVertex::isValid() const {
    return isShortnameValid() && (id != 0);
}

bool Graph::GVertex::operator ==(const GVertex &oVert_) const {
    auto borderColorCompare = (GraphCommon::encodeColor(borderColor) == GraphCommon::encodeColor(oVert_.borderColor));
    auto bgrColorCompare = (GraphCommon::encodeColor(backgroundColor) == GraphCommon::encodeColor(oVert_.backgroundColor));

    auto res = tieFields(oVert_, std::equal_to<>{}) &&
            (std::fabs(posX - oVert_.posX) < std::numeric_limits<double>::epsilon()) &&
            (std::fabs(posY - oVert_.posY) < std::numeric_limits<double>::epsilon()) &&
            borderColorCompare &&
            bgrColorCompare;


    if (!res) {
        LOG_DEBUG_SYNC("EQUAL TIE FIELDS:");
        LOG_DEBUG_SYNC("Values mine:", id, shortName, name, description);
        LOG_DEBUG_SYNC("Values others:", oVert_.id, oVert_.shortName, oVert_.name, oVert_.description);
        LOG_DEBUG_SYNC("Position equals:",
                        (std::fabs(posX - oVert_.posX) < std::numeric_limits<double>::epsilon()),
                        (std::fabs(posY - oVert_.posY) < std::numeric_limits<double>::epsilon()));
        LOG_DEBUG_SYNC("Color equals:", borderColorCompare, bgrColorCompare);
        LOG_DEBUG_SYNC("Colors BORDER:      ", GraphCommon::encodeColor(borderColor), GraphCommon::encodeColor(oVert_.borderColor));
        LOG_DEBUG_SYNC("Colors BACKGROUND:  ", GraphCommon::encodeColor(backgroundColor), GraphCommon::encodeColor(oVert_.backgroundColor));
        LOG_DEBUG_SYNC("TIE FIELDS ===============================");
    }
    return res;
}

bool Graph::GVertex::operator !=(const GVertex &oVert_) const {

    auto borderColorCompare = (GraphCommon::encodeColor(borderColor) != GraphCommon::encodeColor(oVert_.borderColor));
    auto bgrColorCompare = (GraphCommon::encodeColor(backgroundColor) != GraphCommon::encodeColor(oVert_.backgroundColor));

    auto res = tieFields(oVert_, std::not_equal_to<>{}) ||
            (std::fabs(posX - oVert_.posX) > std::numeric_limits<double>::epsilon()) ||
            (std::fabs(posY - oVert_.posY) > std::numeric_limits<double>::epsilon()) ||
            borderColorCompare ||
            bgrColorCompare;

    if (!res) {
        LOG_DEBUG_SYNC("NOT EQUAL TIE FIELDS:");
        LOG_DEBUG_SYNC("Values mine:", id, shortName, name, description);
        LOG_DEBUG_SYNC("Values others:", oVert_.id, oVert_.shortName, oVert_.name, oVert_.description);
        LOG_DEBUG_SYNC("Position equals:",
                        (std::fabs(posX - oVert_.posX) > std::numeric_limits<double>::epsilon()),
                        (std::fabs(posY - oVert_.posY) > std::numeric_limits<double>::epsilon()));
        LOG_DEBUG_SYNC("Color equals:", borderColorCompare, bgrColorCompare);
        LOG_DEBUG_SYNC("Colors BORDER:      ", GraphCommon::encodeColor(borderColor), GraphCommon::encodeColor(oVert_.borderColor));
        LOG_DEBUG_SYNC("Colors BACKGROUND:  ", GraphCommon::encodeColor(backgroundColor), GraphCommon::encodeColor(oVert_.backgroundColor));
        LOG_DEBUG_SYNC("TIE FIELDS ===============================");
    }
    return res;
}
