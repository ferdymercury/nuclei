#ifndef ENSDF_H
#define ENSDF_H
#include <QStringList>

class ENSDF
{
public:
    explicit ENSDF(int A);

    QStringList name() const;

    static QStringList aValues();
    QStringList daughterNuclides() const;
    QMap<QString, QString> decays(const QString &daughterNuclide) const;

private:
    static QString path;

    const int a;
    QStringList daughternuclides;

    QStringList contents;
};

#endif // ENSDF_H
