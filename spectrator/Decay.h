#ifndef DECAY_H
#define DECAY_H

#include <stdint.h>
#include <QObject>
#include <QStringList>
#include <QMap>
#include "Nuclide.h"
#include "EnergyLevel.h"

class QGraphicsScene;

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

    ~Decay();

    QString decayTypeAsText() const;
    QGraphicsScene * levelPlot();

    QString toText() const;

signals:
    
public slots:

private:
    void processENSDFLevels() const;

    Nuclide pNuc, dNuc;
    Type t;

    uint64_t parentDecayStartEnergyEv;
    SpinParity parentDecayStartSpin;
    mutable double normalizeDecIntensToPercentParentDecay;


    QStringList ensdf;
    mutable QMap<unsigned int, EnergyLevel*> levels;
    QGraphicsScene *scene;

    static const double outerGammaMargin; // margin between level texts (spin, energy) and gammas
    static const double outerLevelTextMargin; // level lines extend beyond the beginning/end of the level texts by this value
    static const double maxExtraLevelDistance; // maximal additional distance between two level lines
    static const double levelToHalfLifeDistance; // distance between level line and half life text
    static const double parentNuclideLevelLineLength;
    static const double parentNuclideLevelLineExtraLength; // additional length of the decaying level
    static const double arrowHeadLength;
    static const double arrowGap;
    static const double parentNuclideToEnergyLevelsDistance;
};

#endif // DECAY_H
