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
    case TX:
        color = Qt::blue;
        break;
    case RX:
        color = Qt::green;
        break;
    case INFO:
        color = Qt::black;
        break;
    case WARNING:
        color = QColor(255, 165, 0); // 橙色
        break;
    case ERROR:
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
