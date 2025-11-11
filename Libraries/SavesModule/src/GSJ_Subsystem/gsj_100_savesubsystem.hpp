#pragma once

#include "../abstractsavesubsystem.hpp"

namespace Filework {

class GSJ_100_SaveSubsystem : public AbstractSaveSubsystem
{
public:
    GSJ_100_SaveSubsystem();

    // AbstractSaveSubsystem interface
    bool canProcess(const QString &fileData) const override;
    bool createSavedata(const Graph::PMaintainer &pGraph, QString &savedata) const override;
    bool parseSavedata(const Graph::PMaintainer &pGraph, const QString &savedata) const override;

protected:
    /**
     * @brief createSystemJson  Креатор системного JSON
     * @return                  Системный JSON (имеет некоторые нужные поля)
     */
    QJsonObject createSystemJson() const;
};

} // namespace Filework

