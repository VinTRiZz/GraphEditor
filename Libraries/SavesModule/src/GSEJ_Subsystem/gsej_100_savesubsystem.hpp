#pragma once

#include "../abstractsavesubsystem.hpp"
#include "../GSJ_Subsystem/gsj_100_savesubsystem.hpp"

namespace Filework {

class GSEJ_100_SaveSubsystem : public AbstractSaveSubsystem
{
public:
    GSEJ_100_SaveSubsystem();

    void setEncryptionKey(const QString& key);
    QString getEncryptionKey() const;

    // AbstractSaveSubsystem interface
    bool canProcess(const QString &fileData) const;
    bool createSavedata(const Graph::GraphObjectManagerPtr &pGraph, QString &savedata) const;
    bool parseSavedata(const Graph::GraphObjectManagerPtr &pGraph, const QString &savedata) const;

private:
    QString m_key;
    GSJ_100_SaveSubsystem m_gsjSubsys;
};

} // namespace Filework

