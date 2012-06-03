#include "ENSDF.h"
#include <QDir>
#include <QFile>
#include <QChar>
#include <QMap>
#include <QLocale>
#include <iostream>

QString ENSDF::path = "../ensdf";

QStringList ENSDF::name() const
{
    return contents;
}

ENSDF::ENSDF(int A)
    : a(A)
{
    // read data
    QFile f(path + QString("/ensdf.%1").arg(A, int(3), int(10), QChar('0')));
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QString c = QString::fromUtf8(f.readAll());

    contents = c.split('\n');


}

QStringList ENSDF::aValues()
{
    QDir dir(path);
    if (!dir.exists())
        return QStringList();

    QStringList files = dir.entryList(QStringList("ensdf.???"), QDir::Files | QDir::Readable, QDir::Name);

    for (int i=0; i<files.size(); i++)
        files[i] = files.at(i).right(3).remove(QRegExp("^0+"));
    return files;
}

QStringList ENSDF::daughterNuclides() const
{
    QStringList result = contents.filter("ADOPTED LEVELS, GAMMAS");
    for (int i=0; i<result.size(); i++)
        if (result[i].size() >=5)
            result[i][4] = (result[i][4]).toLower();

    result.replaceInStrings(QRegExp("^\\s*([0-9]+)([A-Za-z]+).*$"), "\\2-\\1");
    return result;
}

QMap<QString, QString> ENSDF::decays(const QString &daughterNuclide) const
{
    // recover nuclide identification (NUCID)
    QStringList parts = daughterNuclide.split('-');
    if (parts.size() < 2)
        return QMap<QString, QString>();
    QString nucid(parts.at(1).rightJustified(3, ' '));
    nucid.append(parts.at(0).toUpper().leftJustified(2, ' '));

    QStringList decs = contents.filter(QRegExp("^" + nucid + "\\s{4,4}[\\s0-9]{3,3}[\\sA-Z]{2,2}\\s(B-|B\\+|EC|IT|A\\s)\\sDECAY"));
    QMap<QString, QString> result;
    foreach (QString dec, decs) {
        QString text(dec.mid(12, 2).trimmed());
        if (text.size() == 2)
            text[1] = text.at(1).toLower();
        text.append("-" + dec.mid(9, 3).trimmed() + ", ");
        QString type(dec.mid(15, 2));
        if (type == "EC")
            text.append("Electron Capture");
        else if (type == "B+")
            text.append(QString::fromUtf8("β+"));
        else if (type == "B-")
            text.append(QString::fromUtf8("β-"));
        else if (type == "IT")
            text.append("Isomeric Transition");
        else if (type == "A ")
            text.append("Alpha");
        if (dec.at(24) == '(') {
            QString timeinfo(dec.replace(QRegExp("^.*\\((.+)\\).*$"), "\\1"));
            QStringList timeparts = timeinfo.split(' ');
            if (timeparts.size() >= 2) {
                QLocale clocale("C");
                QLocale loclocale;
                text.append(", T1/2 = " + loclocale.toString(clocale.toFloat(timeparts.at(0))));
                if (timeparts.at(1) == "Y")
                    text.append(" a");
                else if (timeparts.at(1) == "US")
                    text.append(QString::fromUtf8(" µs"));
                else if (timeparts.at(1) == "EV")
                    text.append(" eV");
                else if (timeparts.at(1) == "KEV")
                    text.append(" keV");
                else if (timeparts.at(1) == "MEV")
                    text.append(" MeV");
                else
                    text.append(" " + timeparts.at(1).toLower());
            }
        }
        result[dec] = text;
    }
    return result;
}

//111CD    111AG B- DECAY (7.45 D)
