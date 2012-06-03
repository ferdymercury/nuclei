#ifndef DECAY_H
#define DECAY_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include "Nuclide.h"
#include "EnergyLevel.h"

class Decay : public QObject
{
    Q_OBJECT
public:
    enum Type {
        ElectronCapture,
        BetaPlus,
        BetaMinus,
        IsomericTransition,
        Alpha
    };

    explicit Decay(Nuclide parentNuclide, Nuclide daughterNuclide, Type decayType, QObject *parent = 0);
    explicit Decay(const QStringList &ensdfData, QObject *parent = 0);

    QString decayTypeAsText() const;

    QString toText() const;

signals:
    
public slots:

private:
    double ensdfTimeToSecs(const QString &tstring) const;
    void processENSDFLevels() const;

    Nuclide pNuc, dNuc;
    Type t;

    QStringList ensdf;
    mutable QMap<unsigned int, EnergyLevel> levels;
};

#endif // DECAY_H
