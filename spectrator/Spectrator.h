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

class Spectrator : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Spectrator(QWidget *parent = 0);
    ~Spectrator();

    void updateDockWidth();

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
    bool eventFilter(QObject *obj, QEvent *event);
    bool limitInfoWidth;
    Ui::SpectratorMainWindow *ui;

    ENSDF *currentMassChain;
    QList< QSharedPointer<Decay> > decays;
    QwtPlot *plot;
    QwtPlotCurve *curve;
};

#endif // SPECTRATOR_H
