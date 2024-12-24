#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Dynamic Widgets in Qt");
    window.resize(400, 300);
    window.show();

    return app.exec();
}