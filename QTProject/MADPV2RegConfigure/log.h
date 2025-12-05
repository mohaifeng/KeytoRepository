#ifndef LOG_H
#define LOG_H

#include <QPlainTextEdit>
#include <QDateTime>

class Log
{
public:
    Log();
    ~Log();
public:
    enum LogLevel
    {
        TX,
        RX,
        INFO,
        WARNING,
        ERROR,
    };
    void LogPrint(QPlainTextEdit* edit, const QString& text, LogLevel level);

};

#endif // LOG_H
