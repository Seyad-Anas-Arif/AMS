#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Angle Mesurement");
    window.resize(1280, 720);
    window.show();

    return app.exec();
}
