#ifndef SAVEMASTER_H
#define SAVEMASTER_H

#include "Graph/graphobject.h"

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
};

#endif // SAVEMASTER_H
