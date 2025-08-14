#ifndef GSEJ_FORMAT_H
#define GSEJ_FORMAT_H

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
    ~GSJ_Format();

    /**
     * @brief initFromDataJson  Инициализировать мейнтейнер из JSON. Требует задания мейнтейнера перед вызовом
     * @param iJson             JSON сохранения
     * @return                  true при успешной инициализации
     */
    bool initFromDataJson(const QJsonObject& iJson);

    /**
     * @brief toDataJson    Конвертировать данные мейнтейнера в JSON. Требует задания мейнтейнера перед вызовом
     * @return              объект с заданными полями
     */
    QJsonObject toDataJson() const;

    // Определения интерфейса AbstractSaveFormat
    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;
    bool isFileValid(const QString& targetPath) const override;

protected:    
    /**
     * @brief createSystemJson  Креатор системного JSON
     * @return                  Системный JSON (имеет некоторые нужные поля)
     */
    QJsonObject createSystemJson() const;

    /**
     * @brief isStructureValid  Проверка структуры JSON на валидность
     * @param iJson             JSON для проверки
     * @return                  true если структура может быть использована для инициализации
     */
    bool isStructureValid(const QJsonObject& iJson) const;
};

}  // namespace Filework

#endif  // GSEJ_FORMAT_H
