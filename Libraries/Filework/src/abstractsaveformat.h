#ifndef ABSTRACTSAVEFORMAT_H
#define ABSTRACTSAVEFORMAT_H

#include <GraphObject/Maintainer.h>

#include <QString>

namespace Filework {

class AbstractSaveFormat {
public:
    /**
     * @brief AbstractSaveFormat    Конструктор класса формата
     * @param formatVersion         Версия формата (любой удобный набор символов)
     * @param formatExtension       Расширение формата в виде "ext"
     * @param formatDescription     Описание формата (желательно с указанием расширения)
     * @param backwardCompatible    Является ли формат обратно совместимым с общепринятыми форматами (имеет ли уникальные объекты)
     */
    AbstractSaveFormat(const QString& formatVersion,
                       const QString& formatExtension,
                       const QString& formatDescription,
                       bool backwardCompatible = false);
    virtual ~AbstractSaveFormat();

    /**
     * @brief isBackwardCompatible  Проверка, что формат обратно совместимый с общепринятыми форматами (без уникальных объектов)
     * @return
     */
    bool isBackwardCompatible() const;

    /**
     * @brief getExtension  Получить расширение формата
     * @return              Расширение в виде "ext"
     */
    QString getExtension() const;

    /**
     * @brief getDescription    Получить описание формата
     * @return                  Описание формата
     */
    QString getDescription() const;

    /**
     * @brief getVersion    Получить версию формата
     * @return              Набор символов, не обязательно цифровой
     */
    QString getVersion() const;

    /**
     * @brief setGraphMaintainer Задать мейнтейнер графа
     * @param pGraphMaintaner    Майнтейнер графа
     */
    virtual void setGraphMaintainer(Graph::PMaintainer pGraphMaintaner);

    /**
     * @brief getGraphMaintainer Получить мейнтейнер графа
     * @return                   Мейнтейнер графа. Если не был задан, будет не валидным
     */
    Graph::PMaintainer getGraphMaintainer() const;

    virtual bool save(const QString& targetPath) const = 0;
    virtual bool load(const QString& targetPath) = 0;

    virtual bool isFileValid(const QString& targetPath) const = 0;

private:
    Graph::PMaintainer m_pGraphMaintaner;
    QString m_formatVersion;
    QString m_formatExtension;
    QString m_formatDescription;
    bool    m_isBackwardCompatible;

protected:

    /**
     * @brief replaceFileData   Заменить данные в файле
     * @param filePath          Путь до файла
     * @param iData             Данные, которыми нужно заменить данные в файле
     * @return                  true при успешной замене
     */
    bool replaceFileData(const QString& filePath,
                         const QByteArray& iData) const;

    /**
     * @brief readFromFile  Считать данные из файла
     * @param filePath      Путь до файла
     * @param oData         Массив, в который нужно записать данные файла
     * @return              true если файл существует и был прочитан
     */
    bool readFromFile(const QString& filePath, QByteArray& oData) const;

    /**
     * @brief getEncoded Транслировать символы, которые могут иметь управляющие
     * символы, в base64
     * @param iStr Входной массив байт
     * @return Конвертированный массив байт
     */
    QByteArray getEncoded(const QByteArray& iStr) const;
    QByteArray getDecoded(const QByteArray& iBytes) const;

    /**
     * @brief getEncodedPixmap  Конвертировать pixmap в base64 PNG кодированный
     * набор байт
     * @param iPxmap            Входное изображение
     * @return                  Массив байт
     */
    QByteArray  getEncodedPixmap(const QPixmap& iPxmap) const;
    QPixmap     getDecodedPixmap(const QByteArray& iBytes) const;
};

}  // namespace Filework

#endif  // ABSTRACTSAVEFORMAT_H
