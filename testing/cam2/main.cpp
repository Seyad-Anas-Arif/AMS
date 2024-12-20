/* this folder  is about use treads instreadof timmer not fullt completed*/
#include "CameraApp.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CameraApp cameraApp;
    cameraApp.show();
    return app.exec();
}
