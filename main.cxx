#include "QVTKApplication.h"

#include "mainwindow.h"

int main(int argc, char** argv)
{
    QVTKApplication app(argc, argv);

    QCoreApplication::setOrganizationName("G");
    QCoreApplication::setApplicationName("MNI objects viewer");

    MainWindow w;
    w.show();

    return app.exec();
}
