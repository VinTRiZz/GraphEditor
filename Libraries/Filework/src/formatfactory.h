#ifndef FORMATFACTORY_H
#define FORMATFACTORY_H

#include <QString>
#include <boost/noncopyable.hpp>
#include <set>

#include "abstractsaveformat.h"

namespace Filework {

class FormatFactory {
    FormatFactory();

public:
    static FormatFactory& getInstance();

    /**
     * @brief registerFormat    Регистрация формата. Желательно 1 раз, т.к. они хранятся в векторе
     */
    template <typename T>
    std::enable_if_t<std::is_base_of_v<AbstractSaveFormat, T>, void>
    registerFormat() {
        m_formats.push_back(std::make_shared<T>());
    }

    /**
     * @brief getFormat     Получить формат по расширению
     * @param fileExtension Расширение файла без точки (например "ext")
     * @return              Указатель на формат. Может быть не валидным (если не найден формат)
     */
    const std::shared_ptr<AbstractSaveFormat> getFormat(
        const QString& fileExtension) const;

    /**
     * @brief getAvailableFormats   Получить перечень доступных форматов с описанием
     * @return                      Перечень вида "Описание (РАСШИРЕНИЕ) (*.расширение)"
     */
    QStringList getAvailableFormats() const;

    /**
     * @brief getAvailableExtensions    Получить доступные расширения
     * @return                          Список расширений без точки
     */
    QStringList getAvailableExtensions() const;

    /**
     * @brief getDefaultSaveExtension   Получить расширение по умолчанию. Подразумевает наличие хотя бы одного зарегистрированного
     * @return                          Расширение без точки
     */
    QString getDefaultSaveExtension() const;

private:
    std::vector<std::shared_ptr<AbstractSaveFormat> > m_formats;
};

}  // namespace Filework

#endif  // FORMATFACTORY_H
