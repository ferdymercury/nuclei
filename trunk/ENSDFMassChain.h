#ifndef ENSDFMASSCHAIN_H
#define ENSDFMASSCHAIN_H
#include <QStringList>
#include <QSharedPointer>
#include <QPair>
#include "Decay.h"

class HalfLife;

class ENSDFMassChain
{
public:
    explicit ENSDFMassChain(int A);

    static QStringList aValues();

    QStringList daughterNuclides() const;
    QStringList decays(const QString &daughterNuclideName) const;

    QSharedPointer<Decay> decay(const QString &daughterNuclideName, const QString &decayName);

    friend class Decay;  /// \todo remove!

private:
    typedef QPair<int,int> BlockIndices; // [startidx, size]

    struct ParentRecord {
        QString nuclideName;
        double energy;
        HalfLife hl;
        SpinParity spin;
    };

    static QString nuclideToNucid(const QString &nuclide);
    static QString nucidToNuclide(const QString &nucid);
    static Decay::Type parseDecayType(const QString &tstring);
    static double parseEnsdfEnergy(const QString &estr);
    static ParentRecord parseParentRecord(const QString &precstr);

    void parseBlocks();

    const int a;

    QStringList contents;
    QMap< QString, BlockIndices > m_adoptedlevels; // daughter name: [startidx, size]
    QMap< QString, QMap<QString, BlockIndices > > m_decays; // daughter name: (decay name: [startidx, size])
};

#endif // ENSDF_H
