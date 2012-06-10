#include "EnergyLevel.h"

EnergyLevel::EnergyLevel(uint64_t energyEV, SpinParity spin, HalfLife halfLife, unsigned int isomerNum)
    : e(energyEV), sp(spin), hl(halfLife), isonum(isomerNum), feedintens(std::numeric_limits<double>::quiet_NaN()),
      gragroup(0), graline(0), grafeedarrow(0), graarrowhead(0), graetext(0), graspintext(0), grahltext(0),
      grafeedintens(0)
{
}

uint64_t EnergyLevel::energyEv() const
{
    return e;
}

SpinParity EnergyLevel::spin() const
{
    return sp;
}

HalfLife EnergyLevel::halfLife() const
{
    return hl;
}

unsigned int EnergyLevel::isomerNum() const
{
    return isonum;
}

double EnergyLevel::normalizedFeedIntensity() const
{
    return feedintens;
}

QString EnergyLevel::energyAsText() const
{
    if (e >= 10000000)
        return QString::number(double(e) / 1.E6) + " MeV";
    return QString::number(double(e) / 1.E3) + " keV";
}
