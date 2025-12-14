#pragma once

#include "abstractencryptedformat.h"

namespace Filework {

/**
 * @brief The GSEJ_Format class Формат gsj с шифрованием
 */
class GSEJ_Format : public AbstractEncryptedFormat {
public:
    GSEJ_Format();

protected:
    bool isVersionHigher(const QString& leftV, const QString& rightV) const override;
};

}  // namespace Filework
