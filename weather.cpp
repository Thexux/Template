#include "weather.h"
#include "ui_weather.h"
#include<QToolButton>
#include<QMouseEvent>
#include<QDateTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include<QMessageBox>
#include"WeatherTool.h"

const int MAX = 6;

Weather::Weather(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Weather)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedSize(width(), height());

    connect(ui->exitbtn, &QToolButton::clicked, [=](){
        //cout << 1 << endl;
        qApp->exit(0);
    });

    initdatalist(ui);
    url = "http://t.weather.itboy.net/api/weather/city/";
    cityTmp = city;
    city = u8"绥化";
    manager = new QNetworkAccessManager(this);
    getweatherinfo(manager);

    void(QNetworkAccessManager:: *finish)(QNetworkReply*) = &QNetworkAccessManager::finished;
    connect(manager, finish, [=](QNetworkReply* re){
        QVariant stat = re->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if(re->error() != QNetworkReply::NoError || stat != 200)
        {
            QMessageBox::warning(this, u8"错误", u8"天气：请求数据错误，检查网络连接！", QMessageBox::Ok);
            return;
        }
        QByteArray ba = re->readAll();
        parejosn(ba);
    });


    cout << tool[u8"白银"] << endl;

    QString dateStr = "20231011";
    cout << dateStr.mid(4, 2) << endl;
    QString date = QDate::fromString(dateStr, "yyyyMMdd").toString("yyyy-MM-dd");
    cout << date << endl;

}

void Weather::mouseMoveEvent(QMouseEvent *ev)
{
    //cout << ev->globalPos().x() << ' ' << ev->globalPos().y() << endl;
    move(ev->globalPos() - mpos);
}
void Weather::mousePressEvent(QMouseEvent *ev)
{
    mpos = ev->globalPos() - this->pos();
}

Forecast::Forecast()
{
    for (int i = 0; i < N; i++)
    {
        date[i] = u8"0";
        high[i] = u8"高温 0.0℃";
        low[i] = u8"低温 0.0℃";
        aqi[i] = u8"0";
        type[i] = u8"undefined";
        week[i] = u8"无数据";
    }
}

Today& Today::operator=(const QJsonObject& obj)
{
    QString st = obj.value("date").toString();
    date = st.mid(0, 4) + "-" + st.mid(4, 2) + "-" + st.mid(6);
    city = obj.value("cityInfo").toObject().value("city").toString();

    QJsonObject jsobj = obj.value("data").toObject();
    shidu = jsobj.value("shidu").toString();
    pm25 = QString::number(jsobj.value("pm25").toDouble());
    quality = jsobj.value("quality").toString();
    wendu = jsobj.value("wendu").toString();
    ganmao = jsobj.value("ganmao").toString();

    QJsonObject nowobj = jsobj.value("forecast").toArray()[0].toObject();
    sunrise = nowobj.value("sunrise").toString();
    sunset = nowobj.value("sunset").toString();
    fx = nowobj.value("fx").toString();
    fl = nowobj.value("fl").toString();
    type = nowobj.value("type").toString();
    notice = nowobj.value("notice").toString();
    return *this;
}

Forecast& Forecast::operator=(const QJsonObject& obj)
{
    QJsonArray jsa = obj.value("data").toObject().value("forecast").toArray();

    for (int i = 0; i < min(MAX, jsa.size()); i++)
    {
        QJsonObject jsobj;
        if (i) jsobj = jsa[i - 1].toObject();
        else jsobj = obj.value("data").toObject().value("yesterday").toObject();
        date[i] = jsobj.value("ymd").toString();
        high[i] = jsobj.value("high").toString();
        low[i] = jsobj.value("low").toString();
        aqi[i] = QString::number(jsobj.value("aqi").toDouble());
        type[i] = jsobj.value("type").toString();
        week[i] = jsobj.value("week").toString();
    }
    return *this;
}

void Weather::initdatalist(const Ui::Weather* ui)
{
    forecast_week_list << ui->week0Lb << ui->week1Lb << ui->week2Lb << ui->week3Lb << ui->week4Lb << ui->week5Lb;
    forecast_date_list << ui->date0Lb << ui->date1Lb << ui->date2Lb << ui->date3Lb << ui->date4Lb << ui->date5Lb;
    forecast_aqi_list << ui->qual0Lb << ui->qual1Lb << ui->qual2Lb << ui->qual3Lb << ui->qual4Lb << ui->qual5Lb;
    forecast_type_list << ui->type0Lb << ui->type1Lb << ui->type2Lb << ui->type3Lb << ui->type4Lb << ui->type5Lb;
    forecast_typeIco_list << ui->typeico0Ld << ui->typeico1Ld << ui->typeico2Ld << ui->typeico3Ld << ui->typeico4Ld << ui->typeico5Ld;
    forecast_high_list << ui->wd0Lb << ui->wd2Lb << ui->wd4Lb << ui->wd6Lb << ui->wd8Lb << ui->wd10Lb;
    forecast_low_list << ui->wd1Lb << ui->wd3Lb << ui->wd5Lb << ui->wd7Lb << ui->wd9Lb << ui->wd11Lb;

    for (int i = 0; i < forecast_date_list.size(); i++)
    {
        forecast_week_list[i]->setStyleSheet("background-color: rgba(0, 255, 255, 100);");
        forecast_date_list[i]->setStyleSheet("background-color: rgba(0, 255, 255, 100);");
    }
    ui->citylineEdit->setStyleSheet(
                "QLineEdit {"
                "border: 1px solid gray;"
                "border-radius: 4px; background:argb(47, 47, 47, 130);"
                "color:rgb(255, 255, 255);"
                "}"
                "QLineEdit:hover {"
                "border-color:rgb(101, 255, 106);"
                "}");
}

void Weather::getweatherinfo(QNetworkAccessManager* mg)
{
    QString code = tool[city];
    if(code=="0")
    {
        QMessageBox::warning(this, u8"错误", u8"天气：指定城市不存在！", QMessageBox::Ok);
        return;
    }

    QUrl qurl(url + code);
    mg->get(QNetworkRequest(qurl));
}

void Weather::parejosn(QByteArray ba)
{
    QJsonParseError err;
    QJsonDocument jsdoc = QJsonDocument::fromJson(ba, &err);
    //cout << err.errorString() << endl;
    QJsonObject jsobj = jsdoc.object();
    QString f = jsobj.value("message").toString();
    if (f.contains("success")) today = jsobj, fore = jsobj;
    setlabercontent();

//    cout << today.city << ' ' << today.date << ' ' << today.pm25 << ' ' << today.type << ' ' << today.fl << ' ' << today.fx
//         << ' ' << today.ganmao << ' ' << today.notice << ' ' << today.quality << ' ' << today.shidu << ' ' << today.sunrise << ' '
//         << today.sunset << ' ' << today.wendu << endl;

//    for (int i = 0; i < 6; i++)
//    {
//        cout << fore.aqi[i] << ' ' << fore.date[i] << ' ' << fore.high[i] << ' ' << fore.low[i] << ' ' << fore.type[i] << ' ' << fore.week[i] << endl;
//    }

    //cout << QDateTime::currentDateTime().toString().mid(9, 5) << endl;

}

QString Weather::judgeaqi(int x)
{
    if (x <= 50) return u8"优";
    if (x <= 100) return u8"良";
    if (x <= 150) return u8"轻度";
    if (x <= 200) return u8"中度";
    return u8"重度";
}

void Weather::setlabercontent()
{
    ui->cityLb->setText(today.city);
    ui->datalabel->setText(today.date);
    ui->fltLb->setText(today.fl);
    ui->fxtLb->setText(today.fx);
    ui->pmtLb->setText(today.pm25);
    ui->sdtLb->setText(today.shidu);
    ui->typeLb->setText(today.type);
    ui->gmtext->setText(today.ganmao);
    ui->noticeLb->setText(today.notice);
    ui->qualLb->setText(today.quality);
    ui->qualLb->setStyleSheet(res[judgeaqi(today.quality.toInt())] + "background-color: argb(255, 255, 255, 0);");
    ui->wdLb->setText(today.wendu);

    bool flag = 1;
    if (QDateTime::currentDateTime().toString().mid(9, 5) >= today.sunset) flag = 0;
    //border-image: url(:/day/day/晴.png);
    if (flag) ui->typeicoLd->setStyleSheet(tr("border-image: url(:/day/day/%1.png);").arg(today.type) + "background-color: argb(60, 60, 60, 0);");
    else ui->typeicoLd->setStyleSheet(tr("border-image: url(:/night/night/%1.png);").arg(today.type) + "background-color: argb(60, 60, 60, 0);");

    for (int i = 0; i < MAX; i++)
    {
        if (i > 2) forecast_week_list[i]->setText(fore.week[i]);
        forecast_date_list[i]->setText(fore.date[i].mid(5, 2) + "/" + fore.date[i].mid(8));
        forecast_aqi_list[i]->setText(judgeaqi(fore.aqi[i].toInt()));
        forecast_aqi_list[i]->setStyleSheet(res[judgeaqi(fore.aqi[i].toInt())]);
        forecast_type_list[i]->setText(fore.type[i]);
        if (flag) forecast_typeIco_list[i]->setStyleSheet(tr("image: url(:/day/day/%1.png);").arg(fore.type[i]));
        else forecast_typeIco_list[i]->setStyleSheet(tr("image: url(:/night/night/%1.png);").arg(fore.type[i]));
        forecast_high_list[i]->setText(fore.high[i].mid(3));
        forecast_low_list[i]->setText(fore.low[i].mid(3));
    }

    forecast_week_list[0]->setText(u8"昨天");
    forecast_week_list[1]->setText(u8"今天");
    forecast_week_list[2]->setText(u8"明天");
}


Weather::~Weather()
{
    delete ui;
}

