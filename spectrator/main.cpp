#include <QtGui/QApplication>
#include "Spectrator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Spectrator w;
    w.show();
    
    return a.exec();
}
