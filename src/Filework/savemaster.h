#ifndef SAVEMASTER_H
#define SAVEMASTER_H

#include "Graph/graphobject.h"

#include <QSqlQuery>

/**
 * @brief The SaveMaster class Класс для сохранения и загрузки графов
 */
class SaveMaster
{
public:

    /**
     * @brief save Сохранить граф в файл
     * @param oFilePath Путь до файла для сохранения (файл может не существовать, но его директория -- обязана)
     * @param iGraphObject Объект графа для сохранения
     * @return false если не удалось сохранить файл. В логах будет указано, почему
     */
    static bool save(const QString& oFilePath, const Graph::GraphObject& iGraphObject);

    /**
     * @brief load Выгрузить граф из файла
     * @param iFilePath Путь до файла сохранения
     * @param oGraphObject Объект графа для записи выгруженных данных
     * @return false если не удалось выгрузить граф. В логах будет указано, почему
     */
    static bool load(const QString& iFilePath, Graph::GraphObject& oGraphObject);

    /**
     * @brief testValidance Проверка системы сохранений на корректность работы (для юнит-тестов)
     * @return true если сохранение работает корректно
     */
    static bool testValidance();

private:

    /**
     * @brief executeQuery Выполнить квери в БД, используется во время сохранений
     * @param q Объект квери
     * @param queryText Текст квери
     * @return При ошибке false и вывод в лог текста ошибки
     */
    static bool executeQuery(QSqlQuery &q, const QString& queryText);

    /**
     * @brief getEncoded Транслировать символы, которые могут иметь управляющие символы, в base64
     * @param iStr Входной массив байт
     * @return Конвертированный массив байт
     */
    static QByteArray getEncoded(const QByteArray& iStr);

    /**
     * @brief getEncoded Конвертировать цвет во внутренний способ хранения
     * @param iCol Входной цвет
     * @return Массив байт
     */
    static QByteArray getEncoded(const QColor& iCol);

    /**
     * @brief getEncoded Конвертировать pixmap в base64 PNG кодированный набор байт
     * @param iPxmap Входное изображение
     * @return Массив байт
     */
    static QByteArray getEncoded(const QPixmap& iPxmap);

    /**
     * @brief getDecoded Декодировать массив байт из base64
     * @param iBytes Входной массив байт
     * @return Декодированный массив байт
     */
    static QByteArray getDecoded(const QByteArray& iBytes);

    /**
     * @brief getDecodedPixmap Получение исходного pixmap из закодированного в base64 PNG
     * @param iBytes Входной массив байт
     * @return Декодированный pixmap
     */
    static QPixmap getDecodedPixmap(const QByteArray& iBytes);

    /**
     * @brief getDecodedColor Получить цвет из массива байт внутреннего способа хранения
     * @param iBytes Входной массив байт
     * @return Декодированный цвет
     */
    static QColor getDecodedColor(const QByteArray& iBytes);
};

#endif // SAVEMASTER_H
