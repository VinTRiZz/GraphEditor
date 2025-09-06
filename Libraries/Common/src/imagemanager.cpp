#include "imagemanager.h"

#include <QImage>
#include <QImageReader>

#include <QLabel>

#include <QFile>
#include "encryption.h"

const auto IMAGEMANAGER_PROPERTY_IMAGEHASH {"imagehash"};

ImageManager::ImageManager()
{
    updateCache();
}

ImageManager &ImageManager::getInstance()
{
    static ImageManager inst;
    return inst;
}

void ImageManager::updateCache()
{
    // Кэшируем изображения, возвращаемый результат не интересует
    for (auto& pth : getImageHistoryPaths()) {
        getImageFromPath(pth);
    }
}

QLabel *ImageManager::getLabelFromImage(const std::pair<QString, QImage>& img, const QSize& labelFixedSize) const
{
    auto pxmap =
        QPixmap::fromImage(img.second.scaled(labelFixedSize, Qt::IgnoreAspectRatio));
    auto pLabel = new QLabel;
    pLabel->setFixedSize(labelFixedSize);
    pLabel->setPixmap(pxmap);
    pLabel->setProperty(IMAGEMANAGER_PROPERTY_IMAGEHASH, img.second);
    return pLabel;
}

QString ImageManager::getImageHash(const QWidget *pWidget) const
{
    return pWidget->property(IMAGEMANAGER_PROPERTY_IMAGEHASH).toString();
}

std::pair<QString, QImage> ImageManager::getImageFromPath(const QString& targetPath) {

    if (auto existingHash = m_imagePathCache.find(targetPath); existingHash != m_imagePathCache.end()) {
        return *m_imagesCache.find(existingHash->second);
    }

    QImageReader imgReader;
    imgReader.setAutoDetectImageFormat(true);
    imgReader.setAutoTransform(true);
    imgReader.setDecideFormatFromContent(true);
    imgReader.setFileName(targetPath);

    if (imgReader.imageCount() == 0) {
        LOG_WARNING("Not an image:", targetPath);
        return {};
    }

    auto pxmapHash = calculateImageHash(targetPath);
    m_imagePathCache[targetPath] = pxmapHash;
    m_imagesCache.insert({pxmapHash, {}});
    auto res = m_imagesCache.find(pxmapHash);

    if (imgReader.supportsAnimation()) {  // Для GIF/APNG
        res->second = imgReader.read();
    }
    res->second = QImage(targetPath);  // Для PNG/JPEG/BMP/etc
    return *res;
}

QImage ImageManager::getImageByHash(const QString &hash) const
{
    if (auto targetImg = m_imagesCache.find(hash); targetImg != m_imagesCache.end()) {
        return targetImg->second;
    }

    LOG_WARNING("Not found hash:", hash);
    LOG_DEBUG("Exist:");
    for (auto& [hsh, img] : m_imagesCache) {
        LOG_DEBUG(hsh);
    }
    LOG_DEBUG("==============");

    return {};
}

QString ImageManager::calculateImageHash(const QString &imagePath)
{
    QFile targetFile(imagePath);
    targetFile.open(QIODevice::ReadOnly);
    return Encryption::sha256(targetFile.readAll());
}

QStringList ImageManager::getImageHistoryPaths()
{
    auto imagesDir = DirectoryManager::getTmpDirectory(DirectoryManager::DirectoryTypeTmp::ImportedImages);
    auto imgFiles = imagesDir.entryList();
    imgFiles.removeAll("."); imgFiles.removeAll(".."); // Обход бага кьюта

    QStringList res;
    for (auto& entr : imgFiles) {
        res.push_back(imagesDir.absoluteFilePath(entr));
    }
    return res;
}
