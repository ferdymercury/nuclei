#include "Spectrator.h"
#include "ui_Spectrator.h"
#include "ENSDF.h"


Spectrator::Spectrator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Spectrator),
    currentMassChain(0)
{
    ui->setupUi(this);
    ui->aListWidget->addItems(ENSDF::aValues());
    if (ui->aListWidget->count()) {
        ui->aListWidget->setCurrentRow(0);
        selectedA(ui->aListWidget->item(0)->text());
    }

    connect(ui->aListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedA(QString)));
    connect(ui->nuclideListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(selectedNuclide(QString)));
}

Spectrator::~Spectrator()
{
    delete ui;
}

void Spectrator::selectedA(const QString &a)
{
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

    QMap<QString, QString> decays = currentMassChain->decays(nuclide);
    QMap<QString, QString>::const_iterator i = decays.constBegin();
    while (i != decays.constEnd()) {
        QListWidgetItem *item = new QListWidgetItem(i.value(), ui->decayListWidget);
        item->setData(Qt::UserRole, i.key());
        i++;
    }
}
