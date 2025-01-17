/* PROJECT DETAILES
Project Name: Angle Measurement sensor Main GUI Application
project Discrition: This project is a GUI application that is used to control the camera settings and find the angle in the image.
Status: On going
Remarks : gui related thing completed but still camera calibration and region calibration need to be done

*/
#include <QApplication>
#include "../inc/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);   //for explaining button purpose when it is hovered
    MainWindow w;
    w.show();
    return a.exec();
}