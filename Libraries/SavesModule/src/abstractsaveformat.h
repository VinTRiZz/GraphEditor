#ifndef ABSTRACTSAVEFORMAT_H
#define ABSTRACTSAVEFORMAT_H

#include <GraphObject/Maintainer.h>

#include <QString>

#include "abstractsavesubsystem.hpp"

namespace Filework {

class AbstractSaveFormat : public Graph::MaintainerUserDecorator {
public:
    /**
     * @brief AbstractSaveFormat    Конструктор класса формата
     * @param formatExtension       Расширение формата в виде "ext"
     * @param formatDescription     Описание формата (желательно с указанием
     * расширения)
     * @param backwardCompatible    Является ли формат обратно совместимым с
     * общепринятыми форматами (имеет ли уникальные объекты)
     */
    AbstractSaveFormat(const QString& formatExtension,
                       const QString& formatDescription,
                       bool backwardCompatible = false);
    virtual ~AbstractSaveFormat();

    /**
     * @brief isBackwardCompatible  Проверка, что формат обратно совместимый с
     * общепринятыми форматами (без уникальных объектов)
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
     * @brief save          Сохранить граф в файл
     * @param targetPath    Путь до файла
     * @return              false если произошла ошибка
     */
    virtual bool save(const QString& targetPath) const;
    virtual bool load(const QString& targetPath);

    /**
     * @brief getLastErrorText  Получить текст ошибки
     * @return              Текст последней ошибки
     */
    QString getLastErrorText() const;

private:
    QString m_formatExtension;
    QString m_formatDescription;
    bool m_isBackwardCompatible{false};

    std::shared_ptr<AbstractSaveSubsystem> m_lastUsedSubsystem;
    std::map<QString, std::shared_ptr<AbstractSaveSubsystem> > m_saveSubsystems;

protected:
    void addSubsystem(const std::shared_ptr<AbstractSaveSubsystem>& subsys);
    std::shared_ptr<AbstractSaveSubsystem> getSubsystem(const QString& version) const;

    virtual bool isVersionHigher(const QString& leftV, const QString& rightV) const = 0;

    /**
     * @brief getEncodedPixmap  Конвертировать pixmap в HEX PNG кодированный
     * набор байт
     * @param iPxmap            Входное изображение
     * @return                  Массив байт
     */
    QByteArray getEncodedPixmap(const QPixmap& iPxmap) const;
    QPixmap getDecodedPixmap(const QByteArray& iBytes) const;
};

}  // namespace Filework

#endif  // ABSTRACTSAVEFORMAT_H
