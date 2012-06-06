#include "EnergyLevel.h"

EnergyLevel::EnergyLevel(uint64_t energyEV, Spin spin, double halfLifeSecs)
    : e(energyEV), sp(spin), hlSecs(halfLifeSecs)
{
}

EnergyLevel::EnergyLevel(const EnergyLevel &el)
    : e(el.e), sp(el.sp), hlSecs(el.hlSecs)
{
}

uint64_t EnergyLevel::energyEv() const
{
    return e;
}

EnergyLevel::Spin EnergyLevel::spin() const
{
    return sp;
}

double EnergyLevel::halfLifeSecs() const
{
    return hlSecs;
}

QString EnergyLevel::energyAsText() const
{
    if (e >= 10000000)
        return QString::number(double(e) / 1.E6) + " MeV";
    return QString::number(double(e) / 1.E3) + " keV";
}
