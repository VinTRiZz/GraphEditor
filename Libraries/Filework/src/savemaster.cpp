#include "savemaster.h"

#include "abstractsaveformat.h"

#include "gse_format.h"
#include "gsej_format.h"

#include <Common/Logging.h>

#include <QFileInfo>

QString SaveMaster::formatToDefaultPath(const QString &iPath)
{
    auto fileSuffix = QFileInfo(iPath).completeSuffix();
    if (fileSuffix == "gsej") {
        return iPath;
    }
    return iPath + ".gsej";
}

bool SaveMaster::save(const QString &oFilePath, Graph::GraphObject *iGraphObject)
{
    auto pFormat = getFormat(QFileInfo(oFilePath).completeSuffix());
    pFormat->setGraph(iGraphObject);
    return pFormat->save(oFilePath);
}

bool SaveMaster::load(const QString &iFilePath, Graph::GraphObject *oGraphObject)
{
    auto pFormat = getFormat(QFileInfo(iFilePath).completeSuffix());
    pFormat->setGraph(oGraphObject);
    return pFormat->load(iFilePath);
}

std::shared_ptr<Filework::AbstractSaveFormat> SaveMaster::getFormat(const QString &fileSuffix)
{
    std::shared_ptr<Filework::AbstractSaveFormat> pFormat;
    if (fileSuffix == "gse") {
        pFormat = std::make_shared<Filework::GSE_Format>();
    } else if ((fileSuffix == "gsej") || fileSuffix.isEmpty()) {
        pFormat = std::make_shared<Filework::GSEJ_Format>();
    } else {
        LOG_ERROR("Unknown format got:", fileSuffix);
    }
    return pFormat;
}
