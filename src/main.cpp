#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);   // must exist before any widget is created

    MainWindow window;
    window.show();

    return app.exec();              // runs the event loop until the last window closes
}
