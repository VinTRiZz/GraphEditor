#include "errorobject.hpp"

#include <Components/Logger/Logger.h>

namespace ExtraClasses
{

ErrorObject::ErrorObject(int errCode, const std::string &detailText) :
    std::exception(),
    m_errorCode {errCode},
    m_detailText {detailText}
{

}

const char *ErrorObject::what() const noexcept
{
    m_dumpText = getErrorText() + (m_detailText.empty() ? std::string() : std::string(" (") + m_detailText + ")");
    return m_dumpText.c_str();
}

void ErrorObject::printSelfStd() const
{
    std::cerr << what() << std::endl; // Logger can not be stated or invalid
}

void ErrorObject::printSelf()
{
    COMPLOG_ERROR(m_errorCode, m_detailText);
}

void ErrorObject::printSelf(int errCode, const std::string &detailText)
{
    COMPLOG_ERROR(ErrorObject{}.errorCodeToText(errCode), detailText);
}

void ErrorObject::reset() {
    setCode(ErrorCode::Ok);
}

void ErrorObject::setCode(int errCode)
{
    m_errorCode = errCode;
    if (m_errorCode == ErrorCode::Ok) {
        m_detailText.clear();
    }
}

int ErrorObject::getCode() const
{
    return m_errorCode;
}

bool ErrorObject::isOk() const
{
    return (m_errorCode == ErrorCode::Ok);
}

ErrorObject::operator bool() const
{
    return isOk();
}

std::string ErrorObject::getErrorText() const
{
    return errorCodeToText(m_errorCode);
}

std::string ErrorObject::errorCodeToText(int errc) const
{
    return {};
}

void ErrorObject::setDetailText(const std::string &errText)
{
    m_detailText = errText;
}

std::string ErrorObject::getDetailText() const
{
    return m_detailText;
}

}
