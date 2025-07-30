#ifndef FORMATFACTORY_H
#define FORMATFACTORY_H

#include <boost/noncopyable.hpp>
#include <vector>
#include <QString>

#include "abstractsaveformat.h"

namespace Filework
{

class FormatFactory
{
    FormatFactory();
public:
    static FormatFactory& getInstance();

    const std::shared_ptr<AbstractSaveFormat> getFormat(const QString& fileExtension) const;
    QStringList getAvailableFormats() const;
    QStringList getAvailableExtensions() const;

    QString getDefaultSaveExtension() const;

private:
    std::vector<std::shared_ptr<AbstractSaveFormat> > m_formats;
};

}

#endif // FORMATFACTORY_H
