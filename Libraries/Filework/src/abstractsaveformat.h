#ifndef ABSTRACTSAVEFORMAT_H
#define ABSTRACTSAVEFORMAT_H

#include <QString>
#include <QJsonObject>

#include <GraphObject/Object.h>

namespace Filework
{

class AbstractSaveFormat
{
public:
    AbstractSaveFormat();

    bool fromJson(const QJsonObject& obj);
    QJsonObject toJson() const;

    bool setGraph(Graph::GraphObject* pGraph);
    Graph::GraphObject* getGraph() const;

    virtual bool save(const QString& targetPath) const = 0;
    virtual bool load(const QString& targetPath) = 0;

    virtual bool isFileValid(const QString& targetPath) const = 0;
};

}

#endif // ABSTRACTSAVEFORMAT_H
