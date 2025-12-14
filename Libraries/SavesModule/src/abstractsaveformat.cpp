#include "abstractsaveformat.h"

#include <QBuffer>
#include <QFile>
#include <QPixmap>

#include <Components/Filework/Common.h>
#include <Components/Logger/Logger.h>

namespace Filework {

AbstractSaveFormat::AbstractSaveFormat(const QString& formatExtension,
                                       const QString& formatDescription,
                                       bool backwardCompatible)
    : m_formatExtension(formatExtension)
    , m_formatDescription(formatDescription)
    , m_isBackwardCompatible(backwardCompatible) {}

AbstractSaveFormat::~AbstractSaveFormat() {}

bool AbstractSaveFormat::isBackwardCompatible() const {
    return m_isBackwardCompatible;
}

QString AbstractSaveFormat::getExtension() const {
    return m_formatExtension;
}

QString AbstractSaveFormat::getDescription() const {
    return m_formatDescription;
}

bool AbstractSaveFormat::save(const QString &targetPath) const
{
    QFileInfo targetFile(targetPath);
    auto tDir = targetFile.dir();
    if (!tDir.exists() || !tDir.isReadable()) {
        LOG_ERROR("[SAVE FORMAT] Can not read directory of file:", targetPath);
        return false;
    }

    QString fileDataQt;
    auto maxVers = std::max_element(m_saveSubsystems.begin(), m_saveSubsystems.end(), [this](auto& lhs, auto& rhs){
        return isVersionHigher(lhs.first, rhs.first);
    });
    if (!maxVers->second->createSavedata(getGraph(), fileDataQt)) {
        LOG_ERROR("[SAVE FORMAT] Can not create save data");
        return false;
    }
    return Filework::Common::replaceFileData(targetPath.toStdString(), fileDataQt.toStdString());
}

bool AbstractSaveFormat::load(const QString &targetPath)
{
    std::string fileData;
    if (!Filework::Common::readFileData(targetPath.toStdString(), fileData)) {
        LOG_ERROR("[SAVE FORMAT] Can not read saves file:", targetPath);
        return false;
    }
    auto fileDataQt = QString::fromStdString(fileData);
    for (auto& [vers, pSubsys] : m_saveSubsystems) {
        if (pSubsys->canProcess(fileDataQt)) {
            LOG_INFO("[SAVE FORMAT] Processing file data");
            return pSubsys->parseSavedata(getGraph(), fileDataQt);
        }
    }
    LOG_ERROR("[SAVE FORMAT] No available version to process file:", targetPath);
    return false;
}

QString AbstractSaveFormat::getLastErrorText() const
{
    if (m_lastUsedSubsystem) {
        return m_lastUsedSubsystem->getErrorText();
    }
    LOG_WARNING("[SAVE FORMAT] No subsystem used, but asked for error text");
    return {};
}

void AbstractSaveFormat::addSubsystem(const std::shared_ptr<AbstractSaveSubsystem> &subsys)
{
    m_saveSubsystems.emplace(subsys->getVersion(), subsys);
}

std::shared_ptr<AbstractSaveSubsystem> AbstractSaveFormat::getSubsystem(const QString &version) const
{
    return m_saveSubsystems.at(version);
}

QByteArray AbstractSaveFormat::getEncodedPixmap(const QPixmap& iPxmap) const {
    QByteArray bytes;
    QBuffer byteBuff(&bytes);
    if (iPxmap.save(&byteBuff, "PNG")) {
        return bytes.toHex();
    }
    return {};
}

QPixmap AbstractSaveFormat::getDecodedPixmap(const QByteArray& iBytes) const {
    QPixmap pxmap;
    pxmap.loadFromData(QByteArray::fromHex(iBytes), "PNG");
    return pxmap;
}

}  // namespace Filework
