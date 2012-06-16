#ifndef SPECTRATOR_H
#define SPECTRATOR_H

#include <QMainWindow>
#include "Decay.h"

namespace Ui {
class SpectratorMainWindow;
}
class ENSDF;
class QListWidgetItem;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotZoomer;
class QDoubleSpinBox;

class Spectrator : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Spectrator(QWidget *parent = 0);
    ~Spectrator();

private slots:
    void initialize();

    void selectedA(const QString &a);
    void selectedNuclide(const QString &nuclide);
    void selectedDecay(QListWidgetItem* newitem, QListWidgetItem*);

    void updateEnergySpectrum();

    void showAll();
    void showOriginalSize();
    void zoomIn();
    void zoomOut();

    void setPlotLin();
    void setPlotLog();

    void showAbout();
    
private:
    Ui::SpectratorMainWindow *ui;

    ENSDF *currentMassChain;
    QList< QSharedPointer<Decay> > decays;
    QSharedPointer<Decay> decay;

    QDoubleSpinBox *eres;
    QwtPlot *plot;
    QwtPlotZoomer *zoomer;
    QwtPlotCurve *curve;
};

#endif // SPECTRATOR_H
