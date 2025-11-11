#ifndef GSEJE_FORMAT_H
#define GSEJE_FORMAT_H

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

#endif  // GSEJE_FORMAT_H
