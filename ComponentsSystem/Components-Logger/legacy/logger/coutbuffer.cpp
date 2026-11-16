#include "coutbuffer.h"

#include <QString>

namespace LoggingOld
{

void CoutBuffer::setName(const QString &newName)
{
    m_loggerName = newName;
}

void CoutBuffer::setFile(QFile &logFile)
{
    m_logFile.open(
                logFile.handle(),
                QIODevice::Append,
                QFileDevice::FileHandleFlag::DontCloseHandle
                );
    if (!m_logFile.isOpen())
        qDebug() << "Error opening log file double:" << m_logFile.errorString();
}

std::basic_filebuf<char>::int_type CoutBuffer::overflow(std::basic_filebuf<char>::int_type __c)
{
    m_logBuffer += __c;
    m_synchronized = false;
    return 0;
}

int CoutBuffer::sync()
{
    if (!m_synchronized)
    {
        qInfo() << (m_loggerName + " : " + m_logBuffer).toUtf8().data();
        m_logBuffer.clear();
        m_synchronized = true;
    }
    else
        m_synchronized = false;
    return 0;
}

void CoutBuffer::message(QString data)
{
    qInfo() << (m_loggerName + " : " + data).toUtf8().data();
}

}
