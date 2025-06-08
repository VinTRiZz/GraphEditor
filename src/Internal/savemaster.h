#ifndef SAVEMASTER_H
#define SAVEMASTER_H

#include "Graph/graphobject.h"

class SaveMaster
{
    static void save(const QString& oFilePath, const Graph::GraphObject& iGraphObject);
    static void load(const QString& iFilePath, Graph::GraphObject& oGraphObject);
};

#endif // SAVEMASTER_H
