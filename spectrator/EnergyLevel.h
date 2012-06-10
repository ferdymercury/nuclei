#ifndef ENERGYLEVEL_H
#define ENERGYLEVEL_H

#include <stdint.h>
#include <limits>
#include <QString>
#include "HalfLife.h"
#include "SpinParity.h"

class QGraphicsItemGroup;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class QGraphicsPolygonItem;

class EnergyLevel
{
public:
    EnergyLevel(uint64_t energyEV, SpinParity spin,
                HalfLife halfLife = HalfLife(std::numeric_limits<double>::infinity()),
                unsigned int isomerNum = 0
                );

    uint64_t energyEv() const;
    SpinParity spin() const;
    HalfLife halfLife() const;
    unsigned int isomerNum() const;
    double normalizedFeedIntensity() const;

    QString energyAsText() const;

    friend class Decay;

private:
    uint64_t e;
    SpinParity sp;
    HalfLife hl;
    unsigned int isonum; // >0 for isomeric levels (counted from low energy to high), 0 otherwise
    double feedintens; // says how often this level is directly fed per 100 parent decays

    QGraphicsItemGroup *gragroup;
    QGraphicsLineItem *graline, *grafeedarrow;
    QGraphicsPolygonItem *graarrowhead;
    QGraphicsSimpleTextItem *graetext, *graspintext, *grahltext, *grafeedintens;
};

#endif // ENERGYLEVEL_H
