#ifndef FILE_JSON_H
#define FILE_JSON_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

class JsonReader
{
public:
    JsonReader();
    ~JsonReader();
public:
    QJsonObject  ReadJson(const QString& filePath);
    void Get_Json_Filepath(int dev_type);
public:
    QString filepath;
};

#endif // FILE_JSON_H
