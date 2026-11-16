#pragma once

#include <mutex>
#include <map>

#ifdef QT_CORE_LIB
#include <QString>
#include <QStringList>
#endif // QT_CORE_LIB

namespace ExtraClasses
{


#ifdef QT_CORE_LIB
class AtomicString
{
public:
    AtomicString();
    AtomicString(const char * data);
    AtomicString(const AtomicString & data);
    AtomicString(AtomicString && s);

    AtomicString & operator =(const QString & data);
    AtomicString & operator =(AtomicString && data);
    AtomicString & operator =(const AtomicString & data);
    AtomicString & operator =(const char * data);
    AtomicString & operator +=(const QString data);
    AtomicString & operator +=(const char * data);
    bool operator ==(const QString & other);
    operator QString();

    bool isEmpty();
    QByteArray toUtf8() const;
    QStringList toList() const;

private:
    QString data;
    mutable std::mutex mx;

    QString getData() const;
};
#endif // QT_CORE_LIB

}
