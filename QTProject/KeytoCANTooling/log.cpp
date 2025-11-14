#include "log.h"

Log::Log()
{
}
Log::~Log()
{
}

void Log::LogPrint(QPlainTextEdit* edit, const QString& text, LogLevel level)
{
    QColor color;
    switch(level)
    {
    case LOG_TX:
        color = Qt::blue;
        break;
    case LOG_RX:
        color = Qt::green;
        break;
    case LOG_INFO:
        color = Qt::black;
        break;
    case LOG_WARNING:
        color = QColor(255, 165, 0); // 橙色
        break;
    case LOG_ERROR:
        color = Qt::red;
        break;
    default:
        color = Qt::black;
        break;
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logEntry = QString("[%1]:%2").arg(timestamp).arg(text);
    edit->appendHtml(QString("<font color=\"%1\">%2</font>").arg(color.name()).arg(logEntry));
}
