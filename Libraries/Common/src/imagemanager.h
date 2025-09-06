#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <boost/noncopyable.hpp>

class QLabel;

class ImageManager : public boost::noncopyable
{
    ImageManager();
public:
    static ImageManager& getInstance();

    void updateCache();

    /**
     * @brief scaleImage
     * @param img
     * @return
     */
    QPixmap scaleImage(const QImage& img) const;

    /**
     * @brief pixmapFromPath    Считать изображение
     * @param targetPath        Путь до файла
     * @return                  NULL QImage если не удалось считать и хэш изображения
     */
    std::pair<QString, QImage> getImageFromPath(const QString& targetPath);

    /**
     * @brief imageByHash   Получить изображение по его хэшу
     * @param hash
     * @return              NULL QImage, если такого нет
     */
    QImage getImageByHash(const QString& hash) const;

    /**
     * @brief getImageHash  Вычисляет хеш по SHA-256 содержимого файла
     * @param imagePath     Путь до изображения
     * @return
     */
    static QString calculateImageHash(const QString& imagePath);  // TODO: Вынести в некий манипулятор файлов

    static QStringList getImageHistoryPaths();

private:
    std::unordered_map<QString, QString>m_imagePathCache;   // Словарь ПУТЬ --> ХЕШ
    std::unordered_map<QString, QImage> m_imagesCache;      // Словарь ХЕШ --> ИЗОБРАЖЕНИЕ
};

#endif // IMAGEMANAGER_H
