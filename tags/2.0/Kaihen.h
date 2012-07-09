#ifndef KAIHEN_H
#define KAIHEN_H

#include <QMainWindow>
#include "Decay.h"

namespace Ui {
class KaihenMainWindow;
class PreferencesDialog;
}
class QListWidgetItem;
class QModelIndex;
class QwtPlot;
class QwtPlotIntervalCurve;
class QwtPlotZoomer;
class QwtIntervalSample;
class QDoubleSpinBox;
class SearchDialog;
class DecayCascadeItemModel;
class DecayCascadeFilterProxyModel;
class SearchResultDataSource;

class Kaihen : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Kaihen(QWidget *parent = 0);
    ~Kaihen();

private slots:
    void initialize();

    void loadSelectedDecay(const QModelIndex &index);
    void loadSearchResultCascade(const QModelIndex &index);

    void updateDecayData(Decay::DecayDataSet data);
    void updateEnergySpectrum();

    void svgExport();
    void pdfExport();

    void showAll();
    void showOriginalSize();
    void zoomIn();
    void zoomOut();

    void search();
    void searchFinished(SearchResultDataSource *result);

    void setPlotLin();
    void setPlotLog();

    void showPreferences();
    void showAbout();
    
protected:
    void closeEvent(QCloseEvent * event);

private:
    static QVector<QwtIntervalSample> mergeIntervalData(const QVector<double> &x, const QVector<double> &y1, const QVector<double> &y2);
    void loadDecay(QSharedPointer<Decay> decay);

    Ui::KaihenMainWindow *ui;
    QDialog *preferencesDialog;
    Ui::PreferencesDialog *preferencesDialogUi;

    SearchDialog *searchDialog;

    DecayCascadeItemModel *decaySelectionModel, *searchResultSelectionModel;
    DecayCascadeFilterProxyModel *decayProxyModel, *searchProxyModel;
    QSharedPointer<Decay> m_decay;

    QDoubleSpinBox *eres;
    QwtPlot *plot;
    QwtPlotZoomer *zoomer;
    QwtPlotIntervalCurve *curve, *g1curve, *g2curve;
};

#endif // KAIHEN_H
