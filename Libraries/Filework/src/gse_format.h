#ifndef GSE_FORMAT_H
#define GSE_FORMAT_H

#include <QSqlQuery>

#include "abstractsaveformat.h"

namespace Filework {

/**
 * @brief The GSE_Format class  Старый формат сохранения графов, поддерживает
 * только базовые вершины и соединения
 */
class GSE_Format : public AbstractSaveFormat {
public:
    GSE_Format();
    ~GSE_Format();

    // Определение интерфейса AbstractSaveFormat
    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;
    bool isFileValid(const QString& targetPath) const override;

private:
    /**
     * @brief executeQuery Выполнить квери в БД, используется во время
     * сохранений
     * @param q Объект квери
     * @param queryText Текст квери
     * @return При ошибке false и вывод в лог текста ошибки
     */
    bool executeQuery(QSqlQuery& q, const QString& queryText) const;
};

}  // namespace Filework

#endif  // GSE_FORMAT_H
