#include "main_window.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("BOLT Browser"));
    QCoreApplication::setOrganizationName(QStringLiteral("BOLT"));
    QCoreApplication::setApplicationVersion(QStringLiteral(BOLT_APP_VERSION));

    MainWindow window;
    window.show();
    return app.exec();
}
