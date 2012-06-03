#ifndef ENERGYLEVEL_H
#define ENERGYLEVEL_H

#include <stdint.h>
#include <limits>

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

private:
    uint64_t e;
    Spin sp;
    double hlSecs;
};

#endif // ENERGYLEVEL_H
