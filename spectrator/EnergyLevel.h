#ifndef ENERGYLEVEL_H
#define ENERGYLEVEL_H

#include <stdint.h>
#include <limits>
#include <QString>

class QGraphicsItemGroup;
class QGraphicsLineItem;
class QGraphicsTextItem;

class EnergyLevel
{
public:
    struct Spin {
        Spin() : numerator(0), denominator(1), sign(Undefined) {}
        unsigned int numerator;
        unsigned int denominator;
        enum Sign {
            Plus,
            Minus,
            Undefined
        } sign;
    };

    EnergyLevel(uint64_t energyEV, Spin spin, double halfLifeSecs = std::numeric_limits<double>::infinity());
    EnergyLevel(const EnergyLevel &el);

    uint64_t energyEv() const;
    Spin spin() const;
    double halfLifeSecs() const;

    QString energyAsText() const;

    friend class Decay;

private:
    uint64_t e;
    Spin sp;
    double hlSecs;

    QGraphicsItemGroup *gragroup;
    QGraphicsLineItem *graline;
    QGraphicsTextItem *gratext;
};

#endif // ENERGYLEVEL_H
