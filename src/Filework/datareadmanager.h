#ifndef DATAREADMANAGER_H
#define DATAREADMANAGER_H

#include "Graph/graphobject.h"

namespace Filework
{

class DataReadManager
{
public:
    DataReadManager();

    /**
     * @brief load Выгрузить граф из файла
     * @param iFilePath Путь до файла сохранения
     * @param oGraphObject Объект графа для записи выгруженных данных
     * @return false если не удалось выгрузить граф. В логах будет указано, почему
     */
    bool load(const QString& iFilePath, Graph::GraphObject& oGraphObject);
};

}

#endif // DATAREADMANAGER_H
