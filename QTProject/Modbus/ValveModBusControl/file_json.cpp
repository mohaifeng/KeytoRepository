#include "file_json.h"
#include <QDebug>
#include <QCoreApplication>

JsonReader::JsonReader()
{

}

JsonReader::~JsonReader()
{

}

QJsonObject JsonReader::ReadJson(const QString& filePath)
{
    QJsonObject  result;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return result;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();
    if (error.error != QJsonParseError::NoError)
    {
        return result;
    }
    if (doc.isObject())
    {
        result = doc.object();
    }
    return result;
}

void JsonReader::Get_Json_Filepath(int dev_type)
{
    filepath.clear();
    filepath = QCoreApplication::applicationDirPath();
    switch (dev_type)
    {
    case 0://旋转阀
        filepath.append("/config/valve_modbus_cmd.json");
        break;
    case 1:
        break;
    default:
        break;
    }
}
