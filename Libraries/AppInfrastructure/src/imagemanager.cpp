#include "imagemanager.h"

#include <Components/CustomQt/WaitIndicatorDialog.h>

#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QLabel>

#include <Components/Logger/Logger.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Encryption/Hash.h>

ImageManager::ImageManager() {
    updateCache();
}

ImageManager& ImageManager::getInstance() {
    static ImageManager inst;
    return inst;
}

void ImageManager::updateCache() {
    auto& waitDialog = WaitIndicatorDialog::getInstance();
    waitDialog.setDescription("Анализ кэша...");

    auto imagePaths = getImageHistoryPaths();
    double currentStep = waitDialog.getCurrentPercent();
    auto deltaStep = 30.0 / static_cast<double>(imagePaths.size());

    // Кэшируем изображения, возвращаемый результат не интересует
    for (auto& pth : imagePaths) {
        getImageFromPath(pth);
        currentStep += deltaStep;
        waitDialog.setPercent(currentStep);
    }
}

std::pair<QString, QImage> ImageManager::getImageFromPath(
    const QString& targetPath) {
    if (auto existingHash = m_imagePathCache.find(targetPath);
        existingHash != m_imagePathCache.end()) {
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

    if (res->second.isNull()) {
        LOG_ERROR("Loading image from path:", targetPath);
    } else {
        LOG_OK("Loaded image hash:", res->first);
    }

    return *res;
}

QImage ImageManager::getImageByHash(const QString& hash) const {
    if (auto targetImg = m_imagesCache.find(hash);
        targetImg != m_imagesCache.end()) {
        return targetImg->second;
    }
    return {};
}

QString ImageManager::calculateImageHash(const QString& imagePath) {
    QFile targetFile(imagePath);
    targetFile.open(QIODevice::ReadOnly);
    return Encryption::qtSha256(targetFile.readAll());
}

QStringList ImageManager::getImageHistoryPaths() {
    auto imagesDir = Common::DirectoryManager::getDirectoryStatic(Common::DirectoryManager::DirectoryType::Temporary);
    auto imgFiles = imagesDir.entryList();
    imgFiles.removeAll(".");
    imgFiles.removeAll("..");  // Обход бага кьюта

    QStringList res;
    for (auto& entr : imgFiles) {
        res.push_back(imagesDir.absoluteFilePath(entr));
    }
    return res;
}
