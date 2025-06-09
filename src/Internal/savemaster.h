#ifndef SAVEMASTER_H
#define SAVEMASTER_H

#include "Graph/graphobject.h"

class SaveMaster
{
public:
    static bool save(const QString& oFilePath, const Graph::GraphObject& iGraphObject);
    static bool load(const QString& iFilePath, Graph::GraphObject& oGraphObject);
};

#endif // SAVEMASTER_H
