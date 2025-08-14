#ifndef GSEJE_FORMAT_H
#define GSEJE_FORMAT_H

#include "gsj_format.h"
#include "encryptedsaveformat.h"

namespace Filework {

/**
 * @brief The GSEJ_Format class Формат gsj с шифрованием
 */
class GSEJ_Format : public AbstractSaveFormat, public EncryptedSaveFormat {
public:
    GSEJ_Format();
    ~GSEJ_Format();

    /**
     * @brief setGraphMaintainer    Переопределение задания мейнтейнера для работы с композицией
     * @param pMaintainer           Мейнтейнер
     */
    void setGraphMaintainer(Graph::PMaintainer pMaintainer) override;

    // Определения интерфейса AbstractSaveFormat
    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;
    bool isFileValid(const QString& targetPath) const override;

private:
    QString m_key;              //! Ключ шифрования
    GSJ_Format m_rootFormat;    //! Композиция с JSON форматом, по факту класс GSEJ_Format просто шифрует его данные и добавляет свои

    /**
     * @brief createSystemJson  Создать системный JSON, идентифицирующий формат
     * @return                  Системный JSON
     */
    QJsonObject createSystemJson() const;
};

}  // namespace Filework

#endif  // GSEJE_FORMAT_H
