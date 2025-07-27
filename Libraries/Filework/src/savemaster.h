#ifndef SAVEMASTER_H
#define SAVEMASTER_H

#include <GraphObject/Maintainer.h>

#include <QSqlQuery>

namespace Filework {
class AbstractSaveFormat;
}

/**
 * @brief The SaveMaster class Класс для сохранения и загрузки майнтейнера графов
 */
class SaveMaster
{
public:

    /**
     * @brief formatToDefaultPath   Форматирует путь в путь по умолчанию
     * @param iPath                 Путь до файла сохранений
     * @return                      5Отформатированный путь
     */
    static QString formatToDefaultPath(const QString& iPath);

    static QStringList getAvailableFormats();

    /**
     * @brief save              Сохранить граф в файл
     * @param oFilePath         Путь до файла для сохранения (файл может не существовать, но его директория -- обязана)
     * @param iGraphMaintaner   Поставщик графа для сохранения
     * @return false            если не удалось сохранить файл, например, при неверном расширении. В логах будет указано, почему
     */
    static bool save(const QString& oFilePath, Graph::PMaintainer iGraphMaintaner);

    /**
     * @brief load              Выгрузить граф из файла
     * @param iFilePath         Путь до файла сохранения
     * @param oGraphMaintaner   Поставщик графа для записи выгруженных данных
     * @return false            если не удалось выгрузить граф, например, при неверном расширении. В логах будет указано, почему
     */
    static bool load(const QString& iFilePath, Graph::PMaintainer oGraphMaintaner);

    /**
     * @brief getFormat     Получить указатель на формат, соответствующий файлу
     * @param fileSuffix    Суффикс файла (его расширение)
     * @return              Если суффикс неизвестен, указатель будет пуст. Иначе - с нужным форматом
     */
    static std::shared_ptr<Filework::AbstractSaveFormat> getFormat(const QString& fileSuffix);
};

#endif // SAVEMASTER_H
