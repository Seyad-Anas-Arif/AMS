/* 
PROJECT NAME    : Camera calibration;
STATUS          :COMPLETED
REMARKS         :This file i am using QT timmer to configure display
                 but displaying delay is too much,(3-4s)
                 other then everthing working as expected as.
*/
#include "cameraapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CameraApp cameraApp;
    cameraApp.show();
    return app.exec();
}
