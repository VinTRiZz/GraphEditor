#include "gsj_format.h"

#include <Components/Common/CommonFunctions.h>
#include <Components/Logger/Logger.h>

#include <QColor>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>

#include "GSJ_Subsystem/gsj_200_savesubsystem.hpp"

namespace Filework {

GSJ_Format::GSJ_Format()
    : AbstractSaveFormat("gsj", "Файл сохранения графа", true) {
    addSubsystem(std::make_shared<GSJ_200_SaveSubsystem>());
}

bool GSJ_Format::isVersionHigher(const QString &leftV, const QString &rightV) const
{
    return (leftV > rightV);
}

}  // namespace Filework
