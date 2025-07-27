#include "graphcommon.h"

#include <QMessageBox>

namespace GraphCommon
{

QByteArray encodeColor(const QColor &iCol)
{
    return QString("#%1%2%3")
            .arg(iCol.red(), 2, 16, QLatin1Char('0'))
            .arg(iCol.green(), 2, 16, QLatin1Char('0'))
            .arg(iCol.blue(), 2, 16, QLatin1Char('0'))
            .arg(iCol.alpha(), 2, 16, QLatin1Char('0'))
            .toUpper().toUtf8().data();
}

QColor decodeColor(const QByteArray &iBytes)
{
    if (iBytes.length() != 7) {
        return {};
    }
    return QColor(iBytes.data());
}


}
