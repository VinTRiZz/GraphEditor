#ifndef ABSTRACTSAVEFORMAT_H
#define ABSTRACTSAVEFORMAT_H

#include <QString>

#include <GraphObject/Object.h>

namespace Filework
{

class AbstractSaveFormat
{
public:
    AbstractSaveFormat();
    virtual ~AbstractSaveFormat();

    void setGraph(Graph::GraphObject* pGraph);
    Graph::GraphObject* getGraph() const;

    virtual bool save(const QString& targetPath) const = 0;
    virtual bool load(const QString& targetPath) = 0;

    virtual bool isFileValid(const QString& targetPath) const = 0;

private:
    Graph::GraphObject* m_pGraph {nullptr};
};

}

#endif // ABSTRACTSAVEFORMAT_H
