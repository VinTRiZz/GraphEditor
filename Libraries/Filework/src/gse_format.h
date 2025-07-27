#ifndef GSE_FORMAT_H
#define GSE_FORMAT_H

#include "abstractsaveformat.h"

#include <QSqlQuery>

namespace Filework
{

/**
 * @brief The GSE_Format class  Старый формат сохранения графов, поддерживает только базовые вершины и соединения
 */
class GSE_Format : public AbstractSaveFormat
{
public:
    GSE_Format();
    ~GSE_Format();

    bool save(const QString& targetPath) const override;
    bool load(const QString& targetPath) override;

    bool isFileValid(const QString& targetPath) const override;

private:
    /**
     * @brief executeQuery Выполнить квери в БД, используется во время сохранений
     * @param q Объект квери
     * @param queryText Текст квери
     * @return При ошибке false и вывод в лог текста ошибки
     */
    bool executeQuery(QSqlQuery &q, const QString& queryText) const;

    /**
     * @brief getEncoded Транслировать символы, которые могут иметь управляющие символы, в base64
     * @param iStr Входной массив байт
     * @return Конвертированный массив байт
     */
    QByteArray getEncoded(const QByteArray& iStr) const;

    /**
     * @brief getEncoded Конвертировать pixmap в base64 PNG кодированный набор байт
     * @param iPxmap Входное изображение
     * @return Массив байт
     */
    QByteArray getEncoded(const QPixmap& iPxmap) const;

    /**
     * @brief getDecoded Декодировать массив байт из base64
     * @param iBytes Входной массив байт
     * @return Декодированный массив байт
     */
    QByteArray getDecoded(const QByteArray& iBytes) const;

    /**
     * @brief getDecodedPixmap Получение исходного pixmap из закодированного в base64 PNG
     * @param iBytes Входной массив байт
     * @return Декодированный pixmap
     */
    QPixmap getDecodedPixmap(const QByteArray& iBytes) const;
};

}

#endif // GSE_FORMAT_H
