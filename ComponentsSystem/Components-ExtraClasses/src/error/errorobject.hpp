#pragma once

#include <string>

#include "errorcodes.hpp"

namespace ExtraClasses
{

/**
 * @brief The ErrorObject class Class for error describing
 */
class ErrorObject : public std::exception
{
public:
    ErrorObject() = default;
    ErrorObject(ErrorObject&&) = default;
    ErrorObject& operator =(ErrorObject&&) = default;

    ErrorObject(const ErrorObject&) = default;
    ErrorObject& operator =(const ErrorObject&) = default;

    ErrorObject(int errCode, const std::string& detailText = {});

    // exception interface
    const char *what() const noexcept override;

    // USED IN CONDITIONS WITH NO LOGGER AVAILABLE
    void printSelfStd() const;

    // USED IN CONDITIONS WITH LOGGER "ALIVE"
    void printSelf();
    static void printSelf(
        int errCode,
        const std::string& detailText = {});

    // Reset restores to NoError state (sugar)
    void reset();
    void setCode(int errCode);
    int getCode() const;

    bool isOk() const;
    operator bool() const;

    std::string getErrorText() const;
    virtual std::string errorCodeToText(int errc) const;

    void setDetailText(const std::string& errText);
    std::string getDetailText() const;

private:
    int                 m_errorCode {ErrorCode::Ok};
    std::string         m_detailText;
    mutable std::string m_dumpText; // std::exception inherit workaround (see method what() )
};

/**
 * @brief The ErrorUser class Subclass for error handling
 */
template <typename ErrorClassT>
class ErrorUserBase
{
public:
    bool isValid() const {
        return m_error.isOk();
    }

    ErrorClassT getError() const {
        return m_error;
    }

protected:
    mutable ErrorClassT m_error;
};

}
