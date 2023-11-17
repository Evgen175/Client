#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

QList<QScreen*> screens=QList<QScreen*>();

int main(int argc, char *argv[])
{
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); //основной пит
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
