#ifndef ABSTRACTSAVEFORMAT_H
#define ABSTRACTSAVEFORMAT_H

#include <QString>

#include <GraphObject/Maintainer.h>

namespace Filework
{

class AbstractSaveFormat
{
public:
    AbstractSaveFormat();
    virtual ~AbstractSaveFormat();

    void                    setGraphMaintaner(std::shared_ptr<Graph::GraphMaintaner> pGraphMaintaner);
    std::shared_ptr<Graph::GraphMaintaner>  getGraphMaintaner() const;
    Graph::GraphObject*     getGraph() const;

    virtual bool save(const QString& targetPath) const = 0;
    virtual bool load(const QString& targetPath) = 0;

    virtual bool isFileValid(const QString& targetPath) const = 0;

private:
    std::shared_ptr<Graph::GraphMaintaner> m_pGraphMaintaner;
};

}

#endif // ABSTRACTSAVEFORMAT_H
