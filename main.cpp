#include "simplelableme.h"
#include <QApplication>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("simpel_LabelMe_zh.qm");
    a.installTranslator(&qtTranslator);
    SimpleLableMe w;
    w.show();

    return a.exec();
}
