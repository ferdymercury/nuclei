#ifndef NUCLIDE_H
#define NUCLIDE_H

#include <QString>
#include <limits>

class Nuclide
{
public:
    Nuclide();
    Nuclide(unsigned int A, const QString &element, double halfLifeSecs = std::numeric_limits<double>::infinity());

    unsigned int a() const;
    QString element() const;
    double halfLifeSecs() const;
    QString halfLifeAsText() const;
    QString unformattedName() const;
    QString formattedName() const;
    QString nucid() const;

private:
    unsigned int m_A;
    QString el;
    double hlSecs; // half life in seconds
};

#endif // NUCLIDE_H
