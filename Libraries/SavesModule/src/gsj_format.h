#pragma once

#include <QJsonObject>

#include "abstractsaveformat.h"

namespace Filework {

/**
 * @brief The GSJ_Format class Обновлённый формат сохранения данных,
 * представляет из себя JSON
 */
class GSJ_Format : public AbstractSaveFormat {
public:
    GSJ_Format();

protected:
    bool isVersionHigher(const QString& leftV, const QString& rightV) const override;
};

}  // namespace Filework
