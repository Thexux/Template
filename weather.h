#ifndef WEATHER_H
#define WEATHER_H

#include <QWidget>
#include<QList>
#include<QLabel>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include"WeatherTool.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Weather; }
QT_END_NAMESPACE

class Today
{
public:
    QString date = "0000-00-00";;
    QString wendu = "0";
    QString city = u8"无数据";
    QString shidu = u8"无数据";
    QString pm25 = u8"无数据";
    QString quality = u8"无数据";
    QString ganmao = u8"无数据";
    QString fx = u8"无数据";
    QString fl = u8"无数据";
    QString type = u8"无数据";
    QString sunrise = "00:00";
    QString sunset = "00:00";
    QString notice = u8"无数据";

    Today& operator=(const QJsonObject& obj);
};

class Forecast
{
private:
    static const int N = 10;
public:
    QString date[N];
    QString high[N];
    QString low[N];
    QString aqi[N];
    QString type[N];
    QString week[N];

    Forecast();
    Forecast& operator=(const QJsonObject& obj);
};

class Weather : public QWidget
{
    Q_OBJECT

public:
    Weather(QWidget *parent = nullptr);
    ~Weather();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void initdatalist(const Ui::Weather* ui);
    void getweatherinfo(QNetworkAccessManager* mg);
    void parejosn(QByteArray ba);
    void setlabercontent();
    QString judgeaqi(int x);

private:
    Ui::Weather *ui;
    QPoint mpos;

    Today today;
    Forecast fore;

    QList<QLabel *> forecast_week_list;
    QList<QLabel *> forecast_date_list;
    QList<QLabel *> forecast_aqi_list;
    QList<QLabel *> forecast_type_list;
    QList<QLabel *> forecast_typeIco_list;
    QList<QLabel *> forecast_high_list;
    QList<QLabel *> forecast_low_list;

    QNetworkAccessManager *manager;
    QString url;
    QString city;
    QString cityTmp;

    WeatherTool tool;
    Resource res;
};

#endif // WEATHER_H
