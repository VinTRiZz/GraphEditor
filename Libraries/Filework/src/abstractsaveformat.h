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

    void                    setGraphMaintaner(Graph::PMaintainer pGraphMaintaner);
    Graph::PMaintainer  getGraphMaintaner() const;
    Graph::GraphObject &getGraph() const;

    virtual bool save(const QString& targetPath) const = 0;
    virtual bool load(const QString& targetPath) = 0;

    virtual bool isFileValid(const QString& targetPath) const = 0;

private:
    Graph::PMaintainer m_pGraphMaintaner;

protected:
    bool rewriteFileData(const QString& filePath, const QByteArray& iData) const;
    bool readFromFile(const QString& filePath, QByteArray& oData) const;
};

}

#endif // ABSTRACTSAVEFORMAT_H
