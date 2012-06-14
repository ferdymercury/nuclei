#include "Spectrator.h"
#include "ui_Spectrator.h"

#include <QResizeEvent>
#include <QDoubleSpinBox>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_series_data.h>
#include <qwt_scale_engine.h>
#include <qwt_text.h>

#include "ENSDF.h"

Q_DECLARE_METATYPE( QSharedPointer<Decay> )

class PlotZoomer : public QwtPlotZoomer
{
public:
    PlotZoomer(QwtPlotCanvas *c)
        : QwtPlotZoomer(c)
    {
    };
protected:
    virtual QwtText trackerTextF(const QPointF &p) const
    {
        return QwtText(QString("%1 keV").arg(p.x(), 0, 'f', 1));
    };
};

Spectrator::Spectrator(QWidget *parent) :
    QMainWindow(parent), avoidInfoDockResizeRecursion(false), avoidAnisotropDockResizeRecursion(false),
    ui(new Ui::SpectratorMainWindow),
    currentMassChain(0), zoomer(0)
{
    ui->setupUi(this);
    setCentralWidget(0);
    tabifyDockWidget(ui->decayCascadeDock, ui->energySpectrumDock);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
    ui->decayCascadeDock->raise();

    // add toolbar widgets
    eres = new QDoubleSpinBox(ui->mainToolBar);
    eres->setSuffix(" %");
    eres->setValue(5.0);
    eres->setRange(0.1, 100.0);
    eres->setSingleStep(0.2);
    eres->setToolTip("Energy resolution: FWHM in % at 662 keV");
    connect(eres, SIGNAL(valueChanged(double)), this, SLOT(updateEnergySpectrum()));
    ui->energySpectrumBar->addWidget(eres);

    // workaround
    ui->decayInfoDock->installEventFilter(this);
    ui->anisotropyDock->installEventFilter(this);

    plot = new QwtPlot(ui->energySpectrumDock);
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

    ui->aListWidget->addItems(ENSDF::aValues());
    if (ui->aListWidget->count()) {
        // initialize selection
        ui->aListWidget->setCurrentRow(168);
        selectedA(ui->aListWidget->item(168)->text());
    }

    QFont dockfont = ui->decayCascadeDock->font();
    dockfont.setPointSize(dockfont.pointSize() - 1);
    ui->decayCascadeDock->setFont(dockfont);

    connect(ui->aListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedA(QString)));
    connect(ui->nuclideListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedNuclide(QString)));
    connect(ui->decayListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(selectedDecay(QListWidgetItem*,QListWidgetItem*)));

    connect(ui->actionShow_all, SIGNAL(triggered()), this, SLOT(showAll()));
    connect(ui->actionOriginal_Size, SIGNAL(triggered()), this, SLOT(showOriginalSize()));
    connect(ui->actionZoom_In, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoom_Out, SIGNAL(triggered()), this, SLOT(zoomOut()));

    connect(ui->actionLinear, SIGNAL(triggered()), this, SLOT(setPlotLin()));
    connect(ui->actionLogarithmic, SIGNAL(triggered()), this, SLOT(setPlotLog()));

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

    ui->decayView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // workaround
    resizeDockHelper();
}

Spectrator::~Spectrator()
{
    delete ui;
}

void Spectrator::selectedA(const QString &a)
{
    ui->decayListWidget->clear();
    decays.clear();
    ui->decayView->setScene(0);

    delete currentMassChain;
    currentMassChain = new ENSDF(a.toInt());

    ui->nuclideListWidget->clear();

    ui->nuclideListWidget->addItems(currentMassChain->daughterNuclides());
}

void Spectrator::selectedNuclide(const QString &nuclide)
{
    if (!currentMassChain)
        return;

    ui->decayListWidget->clear();
    decays.clear();
    ui->decayView->setScene(0);

    decays = currentMassChain->decays(nuclide);
    for (int i=0; i<decays.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem(decays.at(i)->toText(), ui->decayListWidget);
        item->setData(Qt::UserRole, QVariant::fromValue(decays.at(i)));
    }
}

void Spectrator::selectedDecay(QListWidgetItem* newitem, QListWidgetItem*)
{
    if (!newitem)
        return;

    decay = newitem->data(Qt::UserRole).value< QSharedPointer<Decay> >();
    decay->setUpdateableUi(ui);
    QGraphicsScene *scene = decay->levelPlot();
    ui->decayView->setScene(scene);
    ui->decayView->setSceneRect(scene->sceneRect().adjusted(-20, -20, 20, 20));

    // update plot
    updateEnergySpectrum();
}

void Spectrator::updateEnergySpectrum()
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

void Spectrator::showAll()
{
    // funny workaround from http://qt-project.org/faq/answer/how_can_i_check_which_tab_is_the_current_one_in_a_tabbed_qdockwidget
    QList<QTabBar *> tabList = findChildren<QTabBar *>();
    if(!tabList.isEmpty()){
        QTabBar *tabBar = tabList.at(0);
        if (tabBar->currentIndex() == 0) {
            QGraphicsScene *scene = ui->decayView->scene();
            if (scene)
                ui->decayView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
            return;
        }
        else if (tabBar->currentIndex() == 1) {
            zoomer->zoom(0);
            return;
        }
    }

    QGraphicsScene *scene = ui->decayView->scene();
    if (scene)
        ui->decayView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    zoomer->zoom(0);
}

void Spectrator::showOriginalSize()
{
    ui->decayView->setTransform(QTransform());
}

void Spectrator::zoomIn()
{
    // funny workaround from http://qt-project.org/faq/answer/how_can_i_check_which_tab_is_the_current_one_in_a_tabbed_qdockwidget
    QList<QTabBar *> tabList = findChildren<QTabBar *>();
    if(!tabList.isEmpty()){
        QTabBar *tabBar = tabList.at(0);
        if (tabBar->currentIndex() == 0) {
            ui->decayView->scale(1.25, 1.25);
            return;
        }
        else if (tabBar->currentIndex() == 1) {
            if ((zoomer->zoomRectIndex() + 1) >= zoomer->zoomStack().size()) {
                const QwtScaleDiv *xs = plot->axisScaleDiv(QwtPlot::xBottom);
                const QwtScaleDiv *ys = plot->axisScaleDiv(QwtPlot::yLeft);
                QRectF cr(xs->lowerBound(), ys->lowerBound(), xs->range(), ys->range());
                cr = cr.normalized();
                zoomer->zoom(QRectF(cr.left()+0.1*cr.width(), cr.top()+0.1*cr.height(), 0.8*cr.width(), 0.8*cr.height()));
            }
            else {
                zoomer->zoom(1);
            }
            return;
        }
    }
    ui->decayView->scale(1.25, 1.25);
    if ((zoomer->zoomRectIndex() + 1) >= zoomer->zoomStack().size()) {
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

void Spectrator::zoomOut()
{
    // funny workaround from http://qt-project.org/faq/answer/how_can_i_check_which_tab_is_the_current_one_in_a_tabbed_qdockwidget
    QList<QTabBar *> tabList = findChildren<QTabBar *>();
    if(!tabList.isEmpty()){
        QTabBar *tabBar = tabList.at(0);
        if (tabBar->currentIndex() == 0) {
            ui->decayView->scale(0.8, 0.8);
            return;
        }
        else if (tabBar->currentIndex() == 1) {
            zoomer->zoom(-1);
            return;
        }
    }
    ui->decayView->scale(0.8, 0.8);
    zoomer->zoom(-1);
}

void Spectrator::setPlotLin()
{
    zoomer->zoom(0);
    ui->actionLogarithmic->setChecked(false);
    plot->setAxisAutoScale(QwtPlot::yLeft);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    zoomer->setZoomBase();
}

void Spectrator::setPlotLog()
{
    zoomer->zoom(0);
    ui->actionLinear->setChecked(false);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
    plot->setAxisScale(QwtPlot::yLeft, 1E-8, 10.0);
    zoomer->setZoomBase();
}

void Spectrator::showAbout()
{
}

void Spectrator::resizeEvent(QResizeEvent *)
{
    resizeDockHelper();
}

void Spectrator::resizeDockHelper()
{
    int w = 0;
    if (!ui->decayInfoDock->isFloating())
        w = ui->decayInfoDock->minimumWidth();
    if (!ui->anisotropyDock->isFloating())
        w = qMax(w, ui->anisotropyDock->minimumWidth());

    if (!avoidInfoDockResizeRecursion && ui->decayInfoDock->isVisible() && !ui->decayInfoDock->isFloating()) {
        avoidInfoDockResizeRecursion = true;
        setAnimated(false);
        if (ui->decayInfoDock->isVisible())
            ui->decayInfoDock->setMaximumWidth(w);

        QApplication::sendPostedEvents();
        QApplication::flush();

        ui->decayInfoDock->setMaximumWidth(524287);
        setAnimated(true);
        avoidInfoDockResizeRecursion = false;
    }
    if (!avoidAnisotropDockResizeRecursion && ui->anisotropyDock->isVisible() && !ui->anisotropyDock->isFloating()) {
        avoidAnisotropDockResizeRecursion = true;
        setAnimated(false);
        if (ui->anisotropyDock->isVisible())
            ui->anisotropyDock->setMaximumWidth(w);

        QApplication::sendPostedEvents();
        QApplication::flush();

        ui->anisotropyDock->setMaximumWidth(524287);
        setAnimated(true);
        avoidAnisotropDockResizeRecursion = false;
    }
}

bool Spectrator::eventFilter(QObject *, QEvent *event)
{
    if (event->type() != QEvent::Show)
        return false;

    resizeDockHelper();

    return false;
}

