#include "Kaihen.h"
#include "ui_Kaihen.h"
#include "ui_PreferencesDialog.h"

#include <QResizeEvent>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QSvgGenerator>
#include <QFileDialog>
#include <QPrinter>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_series_data.h>
#include <qwt_scale_engine.h>
#include <qwt_text.h>
#include <qwt_plot_renderer.h>
#include "version.h"

#include "ENSDFMassChain.h"
#include "ENSDFDownloader.h"

class PlotZoomer : public QwtPlotZoomer
{
public:
    PlotZoomer(QwtPlotCanvas *c)
        : QwtPlotZoomer(c)
    {
    }
protected:
    virtual QwtText trackerTextF(const QPointF &p) const
    {
        return QwtText(QString("%1 keV").arg(p.x(), 0, 'f', 1));
    }
};

Kaihen::Kaihen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KaihenMainWindow),
    pdd(new QDialog(this)), pd(new Ui::PreferencesDialog),
    currentMassChain(0), zoomer(0)
{
    ui->setupUi(this);
    setWindowTitle(QCoreApplication::applicationName() + QString(" ") + QCoreApplication::applicationVersion());

    pd->setupUi(pdd);

    // add toolbar widgets
    eres = new QDoubleSpinBox(ui->mainToolBar);
    eres->setSuffix(" %");
    eres->setRange(1.5, 100.0);
    eres->setSingleStep(0.2);
    eres->setToolTip("Energy resolution: FWHM in % at 662 keV");
    connect(eres, SIGNAL(valueChanged(double)), this, SLOT(updateEnergySpectrum()));
    ui->energySpectrumBar->addWidget(eres);

    plot = new QwtPlot(ui->energySpectrumTab);
    plot->setCanvasBackground(Qt::white);
    ui->energySpectrumLayout->addWidget(plot);
    plot->setAxisTitle(QwtPlot::xBottom, "keV");
    plot->enableAxis(QwtPlot::yLeft, false);

    zoomer = new PlotZoomer(plot->canvas());
    zoomer->setTrackerMode(QwtPlotPicker::AlwaysOn);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    new QwtPlotMagnifier(plot->canvas());

    QwtPlotPanner *panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton(Qt::MidButton);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(plot);
    grid->setMajPen(QPen(Qt::gray));
    grid->setMinPen(QPen(QBrush(Qt::lightGray), 1.0, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));
    grid->enableXMin(true);
    grid->enableYMin(true);

    curve = new QwtPlotCurve;
    curve->attach(plot);
    curve->setStyle(QwtPlotCurve::Lines);
    curve->setPen(Qt::NoPen);
    curve->setBrush(QBrush(QColor(64, 166, 255)));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    connect(ui->aListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedA(QString)));
    connect(ui->nuclideListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedNuclide(QString)));
    connect(ui->decayListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedDecay(QString)));

    connect(ui->actionSVG_Export, SIGNAL(triggered()), this, SLOT(svgExport()));
    connect(ui->actionPDF_Export, SIGNAL(triggered()), this, SLOT(pdfExport()));

    connect(ui->actionShow_all, SIGNAL(triggered()), this, SLOT(showAll()));
    connect(ui->actionOriginal_Size, SIGNAL(triggered()), this, SLOT(showOriginalSize()));
    connect(ui->actionZoom_In, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoom_Out, SIGNAL(triggered()), this, SLOT(zoomOut()));

    connect(ui->actionLinear, SIGNAL(triggered()), this, SLOT(setPlotLin()));
    connect(ui->actionLogarithmic, SIGNAL(triggered()), this, SLOT(setPlotLog()));

    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(showPreferences()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

    ui->decayView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // separate init from constructor to avoid crash on cancel
    QTimer::singleShot(0, this, SLOT(initialize()));
}

Kaihen::~Kaihen()
{
    QSettings s;

    s.beginGroup("preferences");
    s.setValue("fontFamily", pd->fontFamily->currentFont().family());
    s.setValue("fontSize", pd->fontSize->value());
    s.setValue("levelTolerance", pd->levelDiff->value());
    s.setValue("gammaTolerance", pd->gammaDiff->value());
    s.endGroup();

    s.setValue("activeTab", (ui->tabWidget->currentWidget() == ui->decayCascadeTab) ? "decay" : "energy");

    s.setValue("energyScale", ui->actionLinear->isChecked() ? "lin" : "log");

    s.setValue("fwhmResolution", eres->value());

    QListWidgetItem *aItem = ui->aListWidget->currentItem();
    if (aItem)
        s.setValue("selectedA", aItem->text());

    QListWidgetItem *nuclideItem = ui->nuclideListWidget->currentItem();
    if (nuclideItem)
        s.setValue("selectedNuclide", nuclideItem->text());

    QListWidgetItem *decayItem = ui->decayListWidget->currentItem();
    if (decayItem)
        s.setValue("selectedDecay", decayItem->text());

    delete pd;
    delete ui;
}

void Kaihen::initialize()
{
    QSettings s;
    // initialize settings if necessary
    if (!s.contains("exportDir"))
        s.setValue("exportDir", QDir::homePath());

    s.beginGroup("preferences");
    pd->fontFamily->setCurrentFont(QFont(s.value("fontFamily", QFont().family()).toString()));
    pd->fontSize->setValue(s.value("fontSize", 14).toInt());
    pd->levelDiff->setValue(s.value("levelTolerance", 1.0).toDouble());
    pd->gammaDiff->setValue(s.value("gammaTolerance", 1.0).toDouble());
    s.endGroup();

    // load available mass numbers (i.e. search for available ENSDF files)
    QStringList a(ENSDFMassChain::aValues());
    bool firsttry = true;
    while (a.isEmpty()) {
        if (!firsttry)
            if (QMessageBox::Close == QMessageBox::warning(this, "Selected Folder is Empty", "Please select a folder containing the ENSDF database or use the download feature!", QMessageBox::Ok | QMessageBox::Close, QMessageBox::Ok))
                qApp->quit();
        ENSDFDownloader downloader(this);
        if (downloader.exec() != QDialog::Accepted) {
            qApp->quit();
            return;
        }

        a = ENSDFMassChain::aValues();
        firsttry = false;
    }
    ui->aListWidget->addItems(a);

    // restore last session
    eres->setValue(s.value("fwhmResolution", 5.0).toDouble());

    if (s.value("energyScale", "lin").toString() == "log")
        ui->actionLogarithmic->trigger();

    if (s.value("activeTab", "decay").toString() == "decay")
        ui->tabWidget->setCurrentWidget(ui->decayCascadeTab);
    else
        ui->tabWidget->setCurrentWidget(ui->energySpectrumTab);

    QString selectedA(s.value("selectedA").toString());
    QList<QListWidgetItem *> aItems(ui->aListWidget->findItems(selectedA, Qt::MatchExactly));
    if (!aItems.isEmpty())
        ui->aListWidget->setCurrentItem(aItems.at(0));

    QString selectedNuclide(s.value("selectedNuclide").toString());
    QList<QListWidgetItem *> nuclideItems(ui->nuclideListWidget->findItems(selectedNuclide, Qt::MatchExactly));
    if (!nuclideItems.isEmpty())
        ui->nuclideListWidget->setCurrentItem(nuclideItems.at(0));

    QString selectedDecay(s.value("selectedDecay").toString());
    QList<QListWidgetItem *> decayItems(ui->decayListWidget->findItems(selectedDecay, Qt::MatchExactly));
    if (!decayItems.isEmpty())
        ui->decayListWidget->setCurrentItem(decayItems.at(0));

}

void Kaihen::selectedA(const QString &aName)
{
    ui->decayView->setScene(0);
    ui->decayListWidget->clear();

    ui->nuclideListWidget->clear();

    delete currentMassChain;
    currentMassChain = new ENSDFMassChain(aName.toInt());

    ui->nuclideListWidget->addItems(currentMassChain->daughterNuclides());
}

void Kaihen::selectedNuclide(const QString &nuclideName)
{
    if (!currentMassChain)
        return;

    ui->decayView->setScene(0);
    ui->decayListWidget->clear();

    ui->decayListWidget->addItems(currentMassChain->decays(nuclideName));
}

void Kaihen::selectedDecay(const QString &decayName)
{
    if (!currentMassChain)
        return;

    if (decayName.isEmpty())
        return;

    decay = currentMassChain->decay(ui->nuclideListWidget->currentItem()->text(), decayName);
    decay->setUpdateableUi(ui);
    decay->setStyle(pd->fontFamily->currentFont(), pd->fontSize->value());
    decay->setFuzzyLimits(pd->levelDiff->value(), pd->gammaDiff->value());
    QGraphicsScene *scene = decay->levelPlot();
    ui->decayView->setScene(scene);
    ui->decayView->setSceneRect(scene->sceneRect().adjusted(-20, -20, 20, 20));

    // update plot
    updateEnergySpectrum();
}

void Kaihen::updateEnergySpectrum()
{
    if (decay.isNull())
        return;
    QVector<QPointF> data(decay->gammaSpectrum(662.0*eres->value()/100.0));
    curve->setData(new QwtPointSeriesData(data));
    if (ui->actionLinear->isChecked())
        plot->setAxisAutoScale(QwtPlot::yLeft);
    else
        plot->setAxisScale(QwtPlot::yLeft, 1E-8, 10.0);
    if (!data.isEmpty())
        plot->setAxisScale(QwtPlot::xBottom, 0.0, data.last().x());
    zoomer->setZoomBase();
}

void Kaihen::svgExport()
{
    QSettings s;

    if (ui->tabWidget->currentWidget() == ui->decayCascadeTab) {
        if (decay.isNull())
            return;

        QString fn(QFileDialog::getSaveFileName(this, "Save As", s.value("exportDir").toString(), "Scalable Vector Graphics (*.svg)"));
        if (fn.isEmpty())
            return;

        QRectF inrect = ui->decayView->scene()->sceneRect();
        QRectF outrect = inrect.adjusted(-10.0, -10.0, 10.0, 10.0);

        QSvgGenerator svgGen;
        svgGen.setFileName(fn);
        svgGen.setSize(outrect.toRect().size());
        svgGen.setViewBox(outrect);
        svgGen.setTitle("Decay Level Scheme for the decay " + decay->toText());
        svgGen.setDescription(QString::fromUtf8("This scheme was created using Kaihen (" KAIHENURL ")"));

        decay->setShadowEnabled(false);
        QPainter painter(&svgGen);
        ui->decayView->scene()->render(&painter, inrect, inrect);
        decay->setShadowEnabled(true);
    }
    else {
        QString fn(QFileDialog::getSaveFileName(this, "Save As", s.value("exportDir").toString(), "Scalable Vector Graphics (*.svg)"));
        if (fn.isEmpty())
            return;

        QwtPlotRenderer r(this);
        r.setLayoutFlags(QwtPlotRenderer::KeepFrames);
        r.renderDocument(plot, fn, "svg", QSizeF(180.0, 120.0), 300);
    }
}

void Kaihen::pdfExport()
{
    QSettings s;

    if (ui->tabWidget->currentWidget() == ui->decayCascadeTab) {
        if (decay.isNull())
            return;

        QString fn(QFileDialog::getSaveFileName(this, "Save As", s.value("exportDir").toString(), "Portable Document Format (*.pdf)"));
        if (fn.isEmpty())
            return;

        const int scalefactor = 10;
        const double margin = 3.0;

        QRectF inrect = ui->decayView->scene()->sceneRect();
        QRectF outrect = inrect.adjusted(-margin*scalefactor, -margin*scalefactor, margin*scalefactor, margin*scalefactor);

        QPrinter p(QPrinter::HighResolution);
        p.setOutputFileName(fn);
        p.setPageMargins(margin, margin, margin, margin, QPrinter::Millimeter);
        p.setOutputFormat(QPrinter::PdfFormat);
        p.setPaperSize(outrect.toRect().size() / scalefactor, QPrinter::Millimeter);
        p.setDocName("Decay Level Scheme for the decay " + decay->toText());
        p.setCreator(QString("%1 %2 (%3)").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion(), KAIHENURL));

        decay->setShadowEnabled(false);
        QPainter painter(&p);
        ui->decayView->scene()->render(&painter);
        decay->setShadowEnabled(true);
    }
    else {
        QString fn(QFileDialog::getSaveFileName(this, "Save As", s.value("exportDir").toString(), "Portable Document Format (*.pdf)"));
        if (fn.isEmpty())
            return;

        QwtPlotRenderer r(this);
        r.setLayoutFlags(QwtPlotRenderer::KeepFrames);
        r.renderDocument(plot, fn, "pdf", QSizeF(180.0, 120.0), 300);
    }
}

void Kaihen::showAll()
{
    if (ui->tabWidget->currentWidget() == ui->decayCascadeTab) {
        QGraphicsScene *scene = ui->decayView->scene();
        if (scene)
            ui->decayView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        return;
    }
    else {
        zoomer->zoom(0);
        return;
    }
}

void Kaihen::showOriginalSize()
{
    ui->decayView->setTransform(QTransform());
}

void Kaihen::zoomIn()
{
    if (ui->tabWidget->currentWidget() == ui->decayCascadeTab) {
        ui->decayView->scale(1.25, 1.25);
    }
    else {
        if (int(zoomer->zoomRectIndex() + 1) >= zoomer->zoomStack().size()) {
            const QwtScaleDiv *xs = plot->axisScaleDiv(QwtPlot::xBottom);
            const QwtScaleDiv *ys = plot->axisScaleDiv(QwtPlot::yLeft);
            QRectF cr(xs->lowerBound(), ys->lowerBound(), xs->range(), ys->range());
            cr = cr.normalized();
            zoomer->zoom(QRectF(cr.left()+0.1*cr.width(), cr.top()+0.1*cr.height(), 0.8*cr.width(), 0.8*cr.height()));
        }
        else {
            zoomer->zoom(1);
        }
    }
}

void Kaihen::zoomOut()
{
    if (ui->tabWidget->currentWidget() == ui->decayCascadeTab)
        ui->decayView->scale(0.8, 0.8);
    else
        zoomer->zoom(-1);
}

void Kaihen::setPlotLin()
{
    zoomer->zoom(0);
    ui->actionLogarithmic->setChecked(false);
    plot->setAxisAutoScale(QwtPlot::yLeft);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    zoomer->setZoomBase();
    ui->tabWidget->setCurrentWidget(ui->energySpectrumTab);
}

void Kaihen::setPlotLog()
{
    zoomer->zoom(0);
    ui->actionLinear->setChecked(false);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
    plot->setAxisScale(QwtPlot::yLeft, 1E-8, 10.0);
    zoomer->setZoomBase();
    ui->tabWidget->setCurrentWidget(ui->energySpectrumTab);
}

void Kaihen::showPreferences()
{
    pdd->exec();

    // save current selection
    QString aString, nuclideString, decayString;
    QListWidgetItem * aItem = ui->aListWidget->currentItem();
    if (aItem)
        aString = aItem->text();
    QListWidgetItem * nuclideItem = ui->nuclideListWidget->currentItem();
    if (nuclideItem)
        nuclideString = nuclideItem->text();
    QListWidgetItem * decayItem = ui->decayListWidget->currentItem();
    if (decayItem)
        decayString = decayItem->text();

    selectedA(aString);

    QList<QListWidgetItem*> nuclideItems(ui->nuclideListWidget->findItems(nuclideString, Qt::MatchExactly));
    if (!nuclideItems.isEmpty())
        ui->nuclideListWidget->setCurrentItem(nuclideItems.at(0));

    QList<QListWidgetItem*> decayItems(ui->decayListWidget->findItems(decayString, Qt::MatchExactly));
    if (!decayItems.isEmpty())
        ui->decayListWidget->setCurrentItem(decayItems.at(0));
}

void Kaihen::showAbout()
{
    QMessageBox::about(this,
                       QString("About: %1 %2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
                       QString::fromUtf8(KAIHENABOUT "<hr />" LIBAKKABOUT "<hr />" GPL)
                       );
}
