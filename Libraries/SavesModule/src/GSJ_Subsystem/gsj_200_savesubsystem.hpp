#pragma once

#include "../abstractsavesubsystem.hpp"

namespace Filework {

class GSJ_200_SaveSubsystem : public AbstractSaveSubsystem
{
public:
    GSJ_200_SaveSubsystem();

    // AbstractSaveSubsystem interface
    bool canProcess(const QString &fileData) const override;
    bool createSavedata(const Graph::GraphObjectManagerPtr &pGraph, QString &savedata) const override;
    bool parseSavedata(const Graph::GraphObjectManagerPtr &pGraph, const QString &savedata) const override;

protected:
    /**
     * @brief createSystemJson  Креатор системного JSON
     * @return                  Системный JSON (имеет некоторые нужные поля)
     */
    QJsonObject createSystemJson() const;
};

} // namespace Filework

