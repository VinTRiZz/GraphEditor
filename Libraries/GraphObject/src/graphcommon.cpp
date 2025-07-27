#include "graphcommon.h"

#include <Common/Logging.h>

namespace GraphCommon
{

QByteArray encodeColor(const QColor &iCol)
{
    return iCol.name(QColor::HexArgb).toUtf8();
}

QColor decodeColor(const QByteArray &iBytes)
{
    if (iBytes.length() != 9) {
        return {};
    }
    return QColor(iBytes.data());
}

}
