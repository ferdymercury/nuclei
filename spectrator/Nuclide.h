#ifndef NUCLIDE_H
#define NUCLIDE_H

#include <QString>
#include <QMap>
#include <QFont>
#include <limits>
#include "HalfLife.h"

class QGraphicsItem;

class Nuclide
{
public:
    Nuclide();
    Nuclide(unsigned int A, const QString &element, HalfLife halfLife = HalfLife(std::numeric_limits<double>::infinity()));

    unsigned int a() const;
    unsigned int z() const;
    QString element() const;
    HalfLife halfLife() const;
    QString name() const;
    QString nucid() const;

    QGraphicsItem * nuclideGraphicsItem(const QFont &nucFont, const QFont &nucIndexFont);

private:
    unsigned int m_A;
    QString el;
    HalfLife hl;

    static const QMap<QString, unsigned int> elToZ;
    static QMap<QString, unsigned int> initElToZ();
};

#endif // NUCLIDE_H
