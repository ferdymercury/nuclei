#include "EnergyLevel.h"

EnergyLevel::EnergyLevel(uint64_t energyEV, Spin spin, double halfLifeSecs)
    : e(energyEV), sp(spin), hlSecs(halfLifeSecs)
{
}
