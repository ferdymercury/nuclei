#include "Nuclide.h"

Nuclide::Nuclide()
{
}

Nuclide::Nuclide(unsigned int A, const QString &element, double halfLifeSecs)
    : m_A(A), hlSecs(halfLifeSecs)
{
    el = element.toLower();
    if (!el.isEmpty())
        el[0] = el[0].toUpper();
}

unsigned int Nuclide::a() const
{
    return m_A;
}

QString Nuclide::element() const
{
    return el;
}

double Nuclide::halfLifeSecs() const
{
    return hlSecs;
}

QString Nuclide::halfLifeAsText() const
{
    if (hlSecs > 86400. * 365. * 2.)
        return QString("%1 years").arg(hlSecs / (86400. * 365.));
    if (hlSecs > 86400.* 2.)
        return QString("%1 days").arg(hlSecs / 86400.);
    if (hlSecs > 3600.* 2.)
        return QString("%1 h").arg(hlSecs / 3600.);
    if (hlSecs > 60.* 2.)
        return QString("%1 min").arg(hlSecs / 60.);

    if (hlSecs < 1.)
        return QString("%1 ms").arg(hlSecs * 1.E3);
    if (hlSecs < 1.E-3)
        return QString::fromUtf8("%1 µs").arg(hlSecs * 1.E6);
    if (hlSecs < 1.E-6)
        return QString("%1 ns").arg(hlSecs * 1.E9);
    if (hlSecs < 1.E-9)
        return QString("%1 ps").arg(hlSecs * 1.E12);
    if (hlSecs < 1.E-12)
        return QString("%1 fs").arg(hlSecs * 1.E15);
    if (hlSecs < 1.E-15)
        return QString("%1 as").arg(hlSecs * 1.E18);

    return QString("%1 s").arg(hlSecs);
}

QString Nuclide::unformattedName() const
{
    return el + "-" + QString::number(m_A);
}

QString Nuclide::formattedName() const
{
    return QString("<sup>%1</sup>%2").arg(m_A).arg(el);
}

/**
 * Returns the NUCID as defined in the ENSDF manual
 * \return Always returns 5 characters
 */
QString Nuclide::nucid() const
{
    return QString("%1%2").arg(m_A, 3, 10, QChar(' ')).arg(el.leftJustified(2, ' '));
}
