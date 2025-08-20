#include "formatfactory.h"

#include "gse_format.h"
#include "gsej_format.h"
#include "gsj_format.h"

namespace Filework {

FormatFactory::FormatFactory() {

  // Хотелось бы иначе, но никак
  registerFormat<GSE_Format>();
  registerFormat<GSJ_Format>();
  registerFormat<GSEJ_Format>();
}

FormatFactory &FormatFactory::getInstance() {
  static FormatFactory inst;
  return inst;
}

const std::shared_ptr<AbstractSaveFormat>
FormatFactory::getFormat(const QString &fileExtension) const {
  for (auto &pFormat : m_formats) {
    if (fileExtension == pFormat->getExtension()) {
      return pFormat;
    }
  }
  return {};
}

QStringList FormatFactory::getAvailableFormats() const {
  QStringList res;
  for (auto &pFormat : m_formats) {
    res.push_back(QString("%0 (%1) (*.%2)")
                      .arg(pFormat->getDescription(),
                           pFormat->getExtension().toUpper(),
                           pFormat->getExtension()));
  }
  return res;
}

QStringList FormatFactory::getAvailableExtensions() const {
  QStringList res;
  for (auto &pFormat : m_formats) {
    res.push_back(pFormat->getExtension());
  }
  return res;
}

QString FormatFactory::getDefaultSaveExtension() const {
  return m_formats.front()->getExtension();
}

} // namespace Filework
