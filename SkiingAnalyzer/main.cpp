#include "skimainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    // Create and start the UI application.
    QApplication a(argc, argv);
    SkiMainWindow w;
    w.show();

    return a.exec();
}
