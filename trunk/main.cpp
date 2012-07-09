#include <QtGui/QApplication>
#include "Kaihen.h"
#include "ENSDFDataSource.h"
#include "SearchConstraints.h"
#include "ProxyStyle.h"
#include "version.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<Decay::CascadeIdentifier>("CascadeIdentifier");
    qRegisterMetaTypeStreamOperators<Decay::CascadeIdentifier>("CascadeIdentifier");

    qRegisterMetaType<ENSDFTreeItem>("ENSDFTreeItem");
    qRegisterMetaTypeStreamOperators<ENSDFTreeItem>("ENSDFTreeItem");

    qRegisterMetaType<SearchConstraints>("SearchConstraints");
    qRegisterMetaTypeStreamOperators<SearchConstraints>("SearchConstraints");

    QApplication a(argc, argv);
    a.setStyle(new ProxyStyle);
    QCoreApplication::setOrganizationName(QString::fromUtf8("Uni-Göttingen"));
    QCoreApplication::setOrganizationDomain("physik.uni-goettingen.de");
    QCoreApplication::setApplicationName("Kaihen");
    QCoreApplication::setApplicationVersion(QString("%1").arg(VERSION));
    Kaihen w;
    w.show();
    
    return a.exec();
}
