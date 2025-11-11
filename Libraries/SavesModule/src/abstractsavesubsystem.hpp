#pragma once

#include <GraphObject/Maintainer.h>

namespace Filework {

class AbstractSaveSubsystem
{
    QString m_version {"Undefined"};
    mutable QString m_errorText {};
public:
    AbstractSaveSubsystem(const QString& version);

    virtual bool canProcess(const QString& fileData) const = 0;

    QString getVersion() const;
    QString getErrorText() const;

    virtual bool createSavedata(const Graph::PMaintainer& pGraph, QString& savedata) const = 0;
    virtual bool parseSavedata(const Graph::PMaintainer& pGraph, const QString& savedata) const = 0;

protected:
    void setErrorText(const QString& errText) const; // const для константных методов
};

} // namespace Filework
