#include "gsej_format.h"

#include "GSEJ_Subsystem/gsej_200_savesubsystem.hpp"

#include <QFileInfo>
#include <QJsonDocument>

namespace Filework {

GSEJ_Format::GSEJ_Format()
    : AbstractEncryptedFormat("gsej", "Зашифрованный граф", true) {
    addSubsystem(std::make_shared<GSEJ_200_SaveSubsystem>());
}

bool GSEJ_Format::isVersionHigher(const QString &leftV, const QString &rightV) const
{
    return (leftV > rightV);
}

}  // namespace Filework
