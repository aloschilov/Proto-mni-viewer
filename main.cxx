#include "QVTKApplication.h"

#include "mainwindow.h"

int main(int argc, char** argv)
{
    QVTKApplication app(argc, argv);
    MainWindow w;

    w.show();

    return app.exec();
}
