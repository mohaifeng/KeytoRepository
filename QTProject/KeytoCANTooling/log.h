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
        LOG_TX,
        LOG_RX,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };
    void LogPrint(QPlainTextEdit* edit, const QString& text, LogLevel level);
};

#endif // LOG_H
