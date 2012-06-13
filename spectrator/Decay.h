#ifndef DECAY_H
#define DECAY_H

#include <stdint.h>
#include <QObject>
#include <QStringList>
#include <QMap>
#include <QFont>
#include <QPen>
#include "Nuclide.h"
#include "SpinParity.h"

class QGraphicsScene;
class EnergyLevel;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class ClickableItem;

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
    
private slots:
    void itemClicked(ClickableItem *item);

private:
    void alignGraphicsItems();
    void initializeStyle();
    void processENSDFLevels() const;

    Nuclide pNuc, dNuc;
    Type t;

    int64_t parentDecayStartEnergyEv;
    SpinParity parentDecayStartSpin;
    mutable double normalizeDecIntensToPercentParentDecay;
    mutable double normalizeGammaIntensToPercentParentDecay;


    QStringList ensdf;
    mutable QMap<int64_t, EnergyLevel*> levels;
    QGraphicsScene *scene;

    // graphics items
    QGraphicsLineItem *pNucBaseLevel, *pNucStartLevel;
    QGraphicsLineItem *pNucVerticalArrow;
    QGraphicsSimpleTextItem *pNucHl, *pNucBaseEnergy, *pNucEnergy, *pNucSpin;

    // style
    QFont stdFont, stdBoldFont, nucFont, nucIndexFont, parentHlFont, feedIntensityFont, gammaFont;
    QPen levelPen, stableLevelPen, feedArrowPen, intenseFeedArrowPen, gammaPen, intenseGammaPen;

    static const double outerGammaMargin; // margin between level texts (spin, energy) and gammas
    static const double outerLevelTextMargin; // level lines extend beyond the beginning/end of the level texts by this value
    static const double maxExtraLevelDistance; // maximal additional distance between two level lines
    static const double levelToHalfLifeDistance; // distance between level line and half life text
    static const double parentNuclideLevelLineLength;
    static const double parentNuclideLevelLineExtraLength; // additional length of the decaying level
    static const double arrowHeadLength;
    static const double arrowHeadWidth;
    static const double arrowGap;
    static const double parentNuclideToEnergyLevelsDistance;
    static const double highlightWidth;
};

#endif // DECAY_H
