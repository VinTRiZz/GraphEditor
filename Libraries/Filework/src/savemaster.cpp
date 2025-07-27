#include "savemaster.h"

#include "abstractsaveformat.h"

#include "gse_format.h"
#include "gsj_format.h"
#include "gsje_format.h"

#include <Common/Logging.h>

#include <QFileInfo>

QString SaveMaster::formatToDefaultPath(const QString &iPath)
{
    auto fileSuffix = QFileInfo(iPath).completeSuffix();
    if (fileSuffix == "gsj") {
        return iPath;
    }
    return iPath + ".gsj";
}

QStringList SaveMaster::getAvailableFormats()
{
    QStringList res;
    res << "Файл графа (.gsj) (*.gsj)";
    res << "Файл зашифрованного графа (.gsje) (*.gsje)";
    res << "Старый формат графа (.gse) (*.gse)";
    return res;
}

bool SaveMaster::save(const QString &oFilePath, Graph::PMaintainer iGraphMaintaner)
{
    auto pFormat = getFormat(QFileInfo(oFilePath).completeSuffix());
    pFormat->setGraphMaintaner(iGraphMaintaner);
    return pFormat->save(oFilePath);
}

bool SaveMaster::load(const QString &iFilePath, Graph::PMaintainer oGraphMaintaner)
{
    auto pFormat = getFormat(QFileInfo(iFilePath).completeSuffix());
    pFormat->setGraphMaintaner(oGraphMaintaner);
    return pFormat->load(iFilePath);
}

std::shared_ptr<Filework::AbstractSaveFormat> SaveMaster::getFormat(const QString &fileSuffix)
{
    std::shared_ptr<Filework::AbstractSaveFormat> pFormat;
    if (fileSuffix == "gse") {
        pFormat = std::make_shared<Filework::GSE_Format>();

    } else if (fileSuffix == "gsje") {
        pFormat = std::make_shared<Filework::GSJE_Format>();

    } if ((fileSuffix == "gsj") || fileSuffix.isEmpty()) {
        pFormat = std::make_shared<Filework::GSJ_Format>();

    } else {
        LOG_ERROR("Unknown format got:", fileSuffix);
    }
    return pFormat;
}
