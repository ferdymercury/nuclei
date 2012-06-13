#ifndef SPECTRATOR_H
#define SPECTRATOR_H

#include <QMainWindow>
#include "Decay.h"

namespace Ui {
class Spectrator;
}
class ENSDF;
class QListWidgetItem;
class QwtPlot;
class QwtPlotCurve;

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

    void showAll();
    void showOriginalSize();
    void zoomIn();
    void zoomOut();

    void setPlotLin();
    void setPlotLog();

    void showAbout();
    
private:
    Ui::Spectrator *ui;

    ENSDF *currentMassChain;
    QList< QSharedPointer<Decay> > decays;
    QwtPlot *plot;
    QwtPlotCurve *curve;
};

#endif // SPECTRATOR_H
