#ifndef SPECTRATOR_H
#define SPECTRATOR_H

#include <QMainWindow>
#include "Decay.h"

namespace Ui {
class Spectrator;
}
class ENSDF;
class QListWidgetItem;

class Spectrator : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Spectrator(QWidget *parent = 0);
    ~Spectrator();

private slots:
    void selectedA(const QString &a);
    void selectedNuclide(const QString &nuclide);
    void selectedDecay(QListWidgetItem* newitem, QListWidgetItem*);
    
private:
    Ui::Spectrator *ui;

    ENSDF *currentMassChain;
    QList< QSharedPointer<Decay> > decays;
};

#endif // SPECTRATOR_H
