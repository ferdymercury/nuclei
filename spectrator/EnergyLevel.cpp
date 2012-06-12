#include "EnergyLevel.h"
#include <QtAlgorithms>

EnergyLevel::EnergyLevel(int64_t energyEV, SpinParity spin, HalfLife halfLife, unsigned int isomerNum)
    : e(energyEV), sp(spin), hl(halfLife), isonum(isomerNum), feedintens(std::numeric_limits<double>::quiet_NaN()),
      gragroup(0), graline(0), grafeedarrow(0), graarrowhead(0), graetext(0), graspintext(0), grahltext(0),
      grafeedintens(0), graclickarea(0), graYPos(0.0)
{
}

EnergyLevel::~EnergyLevel()
{
    for (int i=0; i<m_depopulatingTransitions.size(); i++)
        delete m_depopulatingTransitions.at(i);
    m_depopulatingTransitions.clear();
    m_populatingTransitions.clear();
}

int64_t EnergyLevel::energyEv() const
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

/**
  * \return Sorted list of transitions, lowest energy gamma first
  */
QList<GammaTransition *> EnergyLevel::depopulatingTransitions()
{
    qSort(m_depopulatingTransitions.begin(), m_depopulatingTransitions.end(), gammaSmallerThan);
    return m_depopulatingTransitions;
}

QString EnergyLevel::energyAsText() const
{
    if (e >= 10000000)
        return QString::number(double(e) / 1.E6) + " MeV";
    return QString::number(double(e) / 1.E3) + " keV";
}

bool EnergyLevel::gammaSmallerThan(const GammaTransition *const g1, const GammaTransition *const g2)
{
    return g1->energyEv() < g2->energyEv();
}
