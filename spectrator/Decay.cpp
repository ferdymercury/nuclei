#include "Decay.h"
#include <QDebug>
#include <QLocale>
#include <cmath>

Decay::Decay(Nuclide parentNuclide, Nuclide daughterNuclide, Type decayType, QObject *parent)
    : QObject(parent), pNuc(parentNuclide), dNuc(daughterNuclide), t(decayType)
{
}

/**
 * Processes a decay header from an ENSDF data file
 */
Decay::Decay(const QStringList &ensdfData, QObject *parent)
    : QObject(parent), ensdf(ensdfData)
{
    Q_ASSERT(!ensdf.isEmpty());

    // parse header
    const QString &head = ensdf.at(0);
    // set type
    QString type(head.mid(15, 2));
    if (type == "EC")
        t = ElectronCapture;
    else if (type == "B+")
        t = BetaPlus;
    else if (type == "B-")
        t = BetaMinus;
    else if (type == "IT")
        t = IsomericTransition;
    else if (type == "A ")
        t = Alpha;
    // create daughter nuclide
    Nuclide d(head.left(3).trimmed().toUInt(), head.mid(3, 2).trimmed());
    dNuc = d;
    // determine parent's half-life
    double hl = std::numeric_limits<double>::quiet_NaN();
    if (head.at(24) == '(') {
        QString timeinfo = head;
        timeinfo.replace(QRegExp("^.*\\((.+)\\).*$"), "\\1");
        hl = ensdfTimeToSecs(timeinfo);
    }
    // parse information from parent record
    int pidx = ensdf.indexOf(QRegExp("^" + head.mid(9, 5) + "\\s\\sP\\s.*$"));
    if (pidx > 0) {
        // overwrite half-life if found here
        QString timeinfo = ensdf.at(pidx).mid(39, 10);
        double t = ensdfTimeToSecs(timeinfo);
        if (std::isfinite(t))
            hl = t;
    }
    // create parent nuclide
    Nuclide p(head.mid(9, 3).trimmed().toUInt(), head.mid(12, 2).trimmed(), hl);
    pNuc = p;
}

QString Decay::decayTypeAsText() const
{
    switch (t) {
    case ElectronCapture:
        return "Electron Capture";
    case BetaPlus:
        return QString::fromUtf8("β+");
    case BetaMinus:
        return QString::fromUtf8("β-");
    case IsomericTransition:
        return "Isomeric Transition";
    case Alpha:
        return "Alpha";
    }
    return "";
}

QString Decay::toText() const
{
    QString result;
    result.append(QString("%1-%2, %3").arg(pNuc.element())
                  .arg(pNuc.a())
                  .arg(decayTypeAsText()));
    if (std::isfinite(pNuc.halfLifeSecs()))
        result.append(", " + pNuc.halfLifeAsText());
    return result;
}

double Decay::ensdfTimeToSecs(const QString &tstring) const
{
    QString tstr(tstring);
    tstr.remove('?');
    QStringList timeparts = tstr.split(' ');
    double hl = std::numeric_limits<double>::quiet_NaN();
    if (timeparts.size() >= 2) {
        QLocale clocale("C");
        bool ok = false;
        hl = clocale.toDouble(timeparts.at(0), &ok);
        if (!ok)
            return std::numeric_limits<double>::infinity();
        if (timeparts.at(1) == "Y")
            hl *= 365. * 86400.;
        else if (timeparts.at(1) == "D")
            hl *= 86400.;
        else if (timeparts.at(1) == "H")
            hl *= 3600.;
        else if (timeparts.at(1) == "M")
            hl *= 60.;
        else if (timeparts.at(1) == "MS")
            hl *= 1.E-3;
        else if (timeparts.at(1) == "US")
            hl *= 1.E-6;
        else if (timeparts.at(1) == "NS")
            hl *= 1.E-9;
        else if (timeparts.at(1) == "PS")
            hl *= 1.E-12;
        else if (timeparts.at(1) == "FS")
            hl *= 1.E-15;
        else if (timeparts.at(1) == "AS")
            hl *= 1.E-18;
    }
    if (tstr.contains("STABLE", Qt::CaseInsensitive))
        hl = std::numeric_limits<double>::infinity();
    return hl;
}

void Decay::processENSDFLevels() const
{
    if (!levels.isEmpty())
        return;

    // process all level sub-blocks

    foreach (const QString &line, ensdf) {
        QLocale clocale("C");
        // process new level
        if (line.startsWith(dNuc.nucid() + "  L ")) {
            // determine energy
            QString estr(line.mid(9, 10));
            estr.remove('(').remove(')');
            double e = clocale.toDouble(estr.trimmed());
            // determine spin
            QString spstr(line.mid(21, 17));
            spstr.remove('(').remove(')');
            EnergyLevel::Spin spin;
            if (spstr.right(1) == "+")
                spin.sign = EnergyLevel::Spin::Plus;
            else if (spstr.right(1) == "-")
                spin.sign = EnergyLevel::Spin::Minus;
            spstr.remove('+').remove('-');
            QStringList fract(spstr.split('/'));
            if (!fract.isEmpty()) {
                spin.numerator = fract.at(0).toUInt();
                if (fract.size() > 1)
                    spin.denominator = fract.at(0).toUInt();
                if (spin.denominator == 0)
                    spin.denominator = 1;
            }
            // determine level's half-life
            QString hlstr(line.mid(39, 10));
            hlstr.remove('(').remove(')');
            double hl = ensdfTimeToSecs(hlstr.trimmed());

            EnergyLevel el(e*1000., spin, hl);
        }
    }
}

