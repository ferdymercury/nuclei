#include <QtGui/QApplication>
#include "Spectrator.h"
#include "version.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(spectrator);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(QString::fromUtf8("Uni-Göttingen"));
    QCoreApplication::setOrganizationDomain("physik.uni-goettingen.de");
    QCoreApplication::setApplicationName("Spectrator");
    QCoreApplication::setApplicationVersion(QString("%1").arg(VERSION));
    Spectrator w;
    w.show();
    
    return a.exec();
}
