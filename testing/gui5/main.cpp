#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);   //for explaining button purpose when it is hovered
    MainWindow w;
    w.show();
    return a.exec();
}