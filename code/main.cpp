#include <QApplication>
#include <QSettings>

#include "UI/MainWindow.h"
#include "EpisodesParser/QCachingLocale/QCachingLocale.h"

int main(int argc, char *argv[]) {
    QCachingLocale cl;

    // Merely instantiating QCachingLocale activates it. Use Q_UNUSED to prevent
    // compiler warnings.
    Q_UNUSED(cl);

    const int RESTART_CODE = 1000;
    int r;

    do {
        QApplication app(argc, argv);

        QCoreApplication::setOrganizationName("WimLeers");
        QCoreApplication::setOrganizationDomain("wimleers.com");
        QCoreApplication::setApplicationName("WPO Analytics");

        MainWindow * mainWindow = new MainWindow();
        mainWindow->show();

        r = app.exec();
    } while (r == RESTART_CODE);

    return r;
}
