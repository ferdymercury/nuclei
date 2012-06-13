#include "Spectrator.h"
#include "ui_Spectrator.h"
#include "ENSDF.h"

Q_DECLARE_METATYPE( QSharedPointer<Decay> )

Spectrator::Spectrator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Spectrator),
    currentMassChain(0)
{
    ui->setupUi(this);
    ui->aListWidget->addItems(ENSDF::aValues());
    if (ui->aListWidget->count()) {
        // initialize selection
        ui->aListWidget->setCurrentRow(168);
        selectedA(ui->aListWidget->item(168)->text());
    }

    connect(ui->aListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedA(QString)));
    connect(ui->nuclideListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedNuclide(QString)));
    connect(ui->decayListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(selectedDecay(QListWidgetItem*,QListWidgetItem*)));

    ui->decayView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
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

    QSharedPointer<Decay> decay = newitem->data(Qt::UserRole).value< QSharedPointer<Decay> >();
    decay->setUpdateableUi(ui);
    QGraphicsScene *scene = decay->levelPlot();
    ui->decayView->setScene(scene);
    ui->decayView->setSceneRect(scene->sceneRect().adjusted(-20, -20, 20, 20));
}
