#include "Spectrator.h"
#include "ui_Spectrator.h"

#include <QResizeEvent>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_series_data.h>
#include <qwt_scale_engine.h>

#include "ENSDF.h"

Q_DECLARE_METATYPE( QSharedPointer<Decay> )

Spectrator::Spectrator(QWidget *parent) :
    QMainWindow(parent), limitInfoWidth(true),
    ui(new Ui::SpectratorMainWindow),
    currentMassChain(0)
{
    ui->setupUi(this);
    this->setCentralWidget(0);
    this->tabifyDockWidget(ui->decayCascadeDock, ui->energySpectrumDock);
    ui->decayCascadeDock->raise();
    ui->decayInfoDock->installEventFilter(this);
    ui->anisotropyDock->installEventFilter(this);

    plot = new QwtPlot(ui->energySpectrumDock);
    plot->setCanvasBackground(Qt::white);
    ui->energySpectrumLayout->addWidget(plot);
    plot->setAxisTitle(QwtPlot::xBottom, "keV");
    plot->enableAxis(QwtPlot::yLeft, false);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(plot);
    grid->setMajPen(QPen(Qt::gray));
    grid->setMinPen(QPen(QBrush(Qt::lightGray), 1.0, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));
    grid->enableXMin(true);
    grid->enableYMin(true);

    curve = new QwtPlotCurve;
    curve->attach(plot);
    curve->setStyle(QwtPlotCurve::Steps);
    curve->setPen(Qt::NoPen);
    curve->setBrush(QBrush(QColor(64, 166, 255)));

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
}

Spectrator::~Spectrator()
{
    delete ui;
}

void Spectrator::updateDockWidth()
{
    // dock width workaround
//    limitInfoWidth = false;
//    ui->decayInfoDock->setMaximumWidth(524287);
//    ui->anisotropyDock->setMaximumWidth(524287);
//    QApplication::processEvents();
//    limitInfoWidth = true;
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

    QSharedPointer<Decay> decay = newitem->data(Qt::UserRole).value< QSharedPointer<Decay> >();
    decay->setUpdateableUi(ui, this);
    QGraphicsScene *scene = decay->levelPlot();
    ui->decayView->setScene(scene);
    ui->decayView->setSceneRect(scene->sceneRect().adjusted(-20, -20, 20, 20));

    // update plot
    QVector<QPointF> data(decay->gammaSpectrum(40.0));
    curve->setData(new QwtPointSeriesData(data));
    if (!data.isEmpty())
        plot->setAxisScale(QwtPlot::xBottom, 0.0, data.last().x());
    plot->replot();
}

void Spectrator::showAll()
{
    QGraphicsScene *scene = ui->decayView->scene();
    if (scene)
        ui->decayView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void Spectrator::showOriginalSize()
{
    ui->decayView->setTransform(QTransform());
}

void Spectrator::zoomIn()
{
    ui->decayView->scale(1.25, 1.25);
}

void Spectrator::zoomOut()
{
    ui->decayView->scale(0.8, 0.8);
}

void Spectrator::setPlotLin()
{
    ui->actionLogarithmic->setChecked(false);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    plot->replot();
}

void Spectrator::setPlotLog()
{
    ui->actionLinear->setChecked(false);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
    plot->replot();
}

void Spectrator::showAbout()
{
}

bool Spectrator::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::Resize)
        return false;

    ui->decayInfoDock->setMaximumWidth(524287);
    ui->anisotropyDock->setMaximumWidth(524287);

    // dock width workaround
    if (limitInfoWidth && ui->decayInfoDock->isVisible()) {
        int w = qMax(ui->decayInfoDock->minimumWidth(), ui->anisotropyDock->minimumWidth());
        if (ui->decayInfoDock->isVisible())
            ui->decayInfoDock->setMaximumWidth(w);
        if (ui->anisotropyDock->isVisible())
            ui->anisotropyDock->setMaximumWidth(w);
        limitInfoWidth = false;
    }

    return false;
}

