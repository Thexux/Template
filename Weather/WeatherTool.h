#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <map>
#include <QFile>
#include <QCoreApplication>
#include<QDebug>
#define cout qDebug()
#define min(a, b) ((a) > (b) ? (b) : (a))


class WeatherTool
{
public:
    WeatherTool()
    {
        QString filename = QCoreApplication:: applicationDirPath ();
        filename += "/citycode-2019-08-23.json";
        //cout << filename << endl;
        QFile file(filename);
        bool ft = file.open(QIODevice::ReadOnly | QIODevice::Text);
        //cout << ft << endl;
        QByteArray json = file.readAll();
        file.close();
        QJsonParseError err;
        QJsonDocument jsondoc = QJsonDocument::fromJson(json, &err);
        //cout << err.errorString() << endl;
        QJsonArray jsona = jsondoc.array();
        for (int i = 0; i < jsona.size(); i++)
        {
            QString code = jsona[i].toObject().value("city_code").toString();
            QString name = jsona[i].toObject().value("city_name").toString();
            if (code.size() > 0) h[name] = code;
        }
    }

    QString operator[](const QString& name)
    {
        if (h.find(name) != h.end()) return h[name];
        if (h.find(name + u8"市") != h.end()) return h[name + u8"市"];
        return "0";
    }


private:
    std::map<QString, QString> h;

};

class Resource
{
public:
    Resource()
    {
        h[u8"优"] = "color: rgb(0, 255, 0);";
        h[u8"良"] = "color: rgb(255, 255, 0);";
        h[u8"轻度"] = "color: rgb(255, 170, 0);";
        h[u8"中度"] = "color: rgb(255, 0, 0);";
        h[u8"重度"] = "color: rgb(170, 0, 0);";
    }
    QString operator[](const QString& name)
    {
        return h[name];
    }

private:
    std::map<QString, QString> h;

};


#endif // WEATHERTOOL_H
