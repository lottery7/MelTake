#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MelTake");
    MainWindow window;
    window.show();
    return app.exec();
}
