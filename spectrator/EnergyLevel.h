#ifndef ENERGYLEVEL_H
#define ENERGYLEVEL_H

#include <stdint.h>
#include <limits>
#include <QString>
#include <QList>
#include "HalfLife.h"
#include "SpinParity.h"
#include "GammaTransition.h"

class ActiveGraphicsItemGroup;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class QGraphicsPolygonItem;
class GammaTransition;

class EnergyLevel
{
public:
    EnergyLevel(int64_t energyEV, SpinParity spin,
                HalfLife halfLife = HalfLife(std::numeric_limits<double>::infinity()),
                unsigned int isomerNum = 0
                );

    ~EnergyLevel();

    int64_t energyEv() const;
    SpinParity spin() const;
    HalfLife halfLife() const;
    unsigned int isomerNum() const;
    double normalizedFeedIntensity() const;

    QList<GammaTransition*> depopulatingTransitions();

    QString energyAsText() const;

    friend class Decay;
    friend class GammaTransition;

private:
    static bool gammaSmallerThan(const GammaTransition * const g1, const GammaTransition * const g2);

    int64_t e;
    SpinParity sp;
    HalfLife hl;
    unsigned int isonum; // >0 for isomeric levels (counted from low energy to high), 0 otherwise
    double feedintens; // says how often this level is directly fed per 100 parent decays

    QList<GammaTransition*> m_populatingTransitions;
    QList<GammaTransition*> m_depopulatingTransitions;

    ActiveGraphicsItemGroup *gragroup;
    QGraphicsLineItem *graline, *grafeedarrow;
    QGraphicsPolygonItem *graarrowhead;
    QGraphicsSimpleTextItem *graetext, *graspintext, *grahltext, *grafeedintens;
    QGraphicsRectItem *graclickarea;
    double graYPos;
};

#endif // ENERGYLEVEL_H
