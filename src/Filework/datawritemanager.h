#ifndef DATAWRITEMANAGER_H
#define DATAWRITEMANAGER_H

#include "Graph/graphobject.h"

namespace Filework
{

class DataWriteManager
{
public:
    DataWriteManager();

    /**
     * @brief save Сохранить граф в файл
     * @param oFilePath Путь до файла для сохранения (файл может не существовать, но его директория -- обязана)
     * @param iGraphObject Объект графа для сохранения
     * @return false если не удалось сохранить файл. В логах будет указано, почему
     */
    bool save(const QString& oFilePath, const Graph::GraphObject& iGraphObject);
};

}

#endif // DATAWRITEMANAGER_H
