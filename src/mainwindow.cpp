#include "mainwindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Beacon Station Manager"));
    resize(1000, 650);

    // A placeholder so you can see the window works. Replace it in Milestone 1.
    // setCentralWidget() makes the window the label's parent, so the window
    // deletes the label when the window itself is deleted. No manual delete needed.
    auto* placeholder = new QLabel(tr("Hello, Qt! Your UI goes here."));
    placeholder->setAlignment(Qt::AlignCenter);
    setCentralWidget(placeholder);
}
