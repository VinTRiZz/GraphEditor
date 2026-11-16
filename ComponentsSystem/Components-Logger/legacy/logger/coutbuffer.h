#ifndef COUTBUFFER_H
#define COUTBUFFER_H

#include <fstream>
#include <QDebug>

#include <QFile>

namespace LoggingOld {

class CoutBuffer : public std::basic_filebuf<char>
{
public:

    CoutBuffer() : std::basic_filebuf<char>() {}

    void setName(const QString& newName);
    void setFile(QFile &logFile);
    int_type overflow(int_type __c);
    int sync();

    void message(QString data);

private:
    QFile m_logFile;
    QString m_logBuffer;
    bool m_synchronized {false};
    QString m_loggerName {"UNKNOWN"};
};

}

#endif // COUTBUFFER_H
