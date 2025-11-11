#include "abstractsavesubsystem.hpp"

namespace Filework {

AbstractSaveSubsystem::AbstractSaveSubsystem(const QString &version) :
    m_version {version}
{

}

QString AbstractSaveSubsystem::getVersion() const
{
    return m_version;
}

QString AbstractSaveSubsystem::getErrorText() const
{
    return m_errorText;
}

void AbstractSaveSubsystem::setErrorText(const QString &errText) const
{
    m_errorText = errText;
}

} // namespace Filework
