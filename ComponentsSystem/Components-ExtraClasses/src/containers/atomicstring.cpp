#include "atomicstring.hpp"

#include <mutex>
#include <map>

#ifdef QT_CORE_LIB
#include <QString>
#include <QStringList>
#endif // QT_CORE_LIB


namespace ExtraClasses
{

#ifdef QT_CORE_LIB
AtomicString::AtomicString()
{
    this->data = QString();
}

AtomicString::AtomicString(const char *data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
}

AtomicString::AtomicString(const AtomicString &data)
{
    mx.lock();
    this->data = data.getData();
    mx.unlock();
}

AtomicString::AtomicString(AtomicString &&s)
{
    mx.lock();
    data = s;
    mx.unlock();
}

AtomicString & AtomicString::operator =(const char *data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

AtomicString & AtomicString::operator +=(const char *data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

bool AtomicString::operator ==(const QString &other)
{
    mx.lock();
    bool result = data == other;
    mx.unlock();
    return result;
}

bool AtomicString::isEmpty()
{
    bool result;
    mx.lock();
    result = data.isEmpty();
    mx.unlock();
    return result;
}

QByteArray AtomicString::toUtf8() const
{
    QByteArray result;
    mx.lock();
    result = data.toUtf8();
    mx.unlock();
    return result;
}

QStringList AtomicString::toList() const
{
    std::string buffer = getData().toUtf8().data();
    QStringList result;

    std::string::iterator spacePosBegin = buffer.begin(), spacePosEnd = buffer.end();

    while (spacePosBegin != buffer.end())
    {
        spacePosEnd = std::find(spacePosBegin, buffer.end(), ' ');

        result << QString::fromStdString(std::string(spacePosBegin, spacePosEnd));

        if (spacePosEnd == buffer.end()) // Not found spaces
            break;
        spacePosBegin = spacePosEnd + 1;
    }
    return result;
}

QString AtomicString::getData() const
{
    QString result;
    mx.lock();
    result = data;
    mx.unlock();
    return data;
}

AtomicString::operator QString()
{
    QString result;
    mx.lock();
    result = data;
    mx.unlock();
    return result;
}

AtomicString & AtomicString::operator +=(const QString data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

AtomicString & AtomicString::operator =(const AtomicString &data)
{
    mx.lock();
    this->data = data.data;
    mx.unlock();
    return *this;
}

AtomicString & AtomicString::operator =(AtomicString &&data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}

AtomicString & AtomicString::operator =(const QString &data)
{
    mx.lock();
    this->data = data;
    mx.unlock();
    return *this;
}
#endif // QT_CORE_LIB

}
