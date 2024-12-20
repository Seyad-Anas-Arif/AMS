/* this  folder is for  using  THread insteadof timmer 75% completed but still its slow only in displaying*/
#include "CameraApp.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CameraApp cameraApp;
    cameraApp.show();
    return app.exec();
}
