#include "QVTKApplication.h"

#include "mainwindow.h"

void myMessageOutput(QtMsgType type, const char *msg)
{
    FILE *file;

    if((file = fopen("debug.txt", "a+")) == NULL)
    {
        return;
    }

    switch (type) {
    case QtDebugMsg:
        fprintf(file, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(file, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(file, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(file, "Fatal: %s\n", msg);
        abort();
    }

    fclose(file);
}

int main(int argc, char** argv)
{
    qInstallMsgHandler(myMessageOutput);

    QVTKApplication app(argc, argv);

    QCoreApplication::setOrganizationName("G");
    QCoreApplication::setApplicationName("MNI objects viewer");

    MainWindow w;
    w.show();

    return app.exec();
}
