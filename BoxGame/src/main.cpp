#include <QApplication>
#include <QSplashScreen>
#include "myview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pix(":/background02.png");
    QSplashScreen splash(pix);
    splash.resize(pix.size());
    splash.show();
    a.processEvents();

    MyView v;
    v.show();
    splash.finish(&v);
    
    return a.exec();
}
