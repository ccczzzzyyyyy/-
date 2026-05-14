#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("保卫萝卜");

    MainWindow window;
    window.setWindowTitle("保卫萝卜 - Carrot Defense");
    window.resize(1100, 700);
    window.show();

    return app.exec();
}
