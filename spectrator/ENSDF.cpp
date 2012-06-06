#include "ENSDF.h"
#include <QDir>
#include <QFile>
#include <QChar>
#include <QMap>
#include <QLocale>
#include "Decay.h"

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

/**
 * Receiver is responsible for deleting the returned objects
 */
QList< QSharedPointer<Decay> > ENSDF::decays(const QString &daughterNuclide) const
{
    // recover nuclide identification (NUCID)
    QStringList parts = daughterNuclide.split('-');
    if (parts.size() < 2)
        return QList< QSharedPointer<Decay> >();
    QString nucid(parts.at(1).rightJustified(3, ' '));
    nucid.append(parts.at(0).toUpper().leftJustified(2, ' '));

    QList< QSharedPointer<Decay> > result;
    int start = -1;
    int stop = 0;
    while ((start = contents.indexOf(QRegExp("^" + nucid + "\\s{4,4}[\\s0-9]{3,3}[\\sA-Z]{2,2}\\s(B-|B\\+|EC|IT|A\\s)\\sDECAY.*"), stop)), start >= 0) {
        stop = contents.indexOf(QRegExp("^\\s*$"), start);
        if (stop < 0)
            stop = contents.size();
        QStringList dec(contents.mid(start, stop-start));
        if (dec.isEmpty())
            continue;
        result.append(QSharedPointer<Decay>(new Decay(dec)));
    }
    return result;
}

