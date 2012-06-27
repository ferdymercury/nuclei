#ifndef DECAY_H
#define DECAY_H

#include <stdint.h>
#include <QObject>
#include <QStringList>
#include <QMap>
#include <QFont>
#include <QPen>
#include "Nuclide.h"
#include "SpinParity.h"
#include "GammaTransition.h"

class QGraphicsScene;
class EnergyLevel;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class ClickableItem;

class Decay : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Undefined,
        ElectronCapture,
        BetaPlus,
        BetaMinus,
        IsomericTransition,
        Alpha
    };

    explicit Decay(const QString &name, Nuclide *parentNuclide, Nuclide *daughterNuclide, Type decayType, QObject *parent = 0);

    ~Decay();

    static QString decayTypeAsText(Type type);

    void setStyle(const QFont &fontfamily, unsigned int sizePx);
    QGraphicsScene * levelPlot();

    void setShadowEnabled(bool enable);

    QString name() const;

    QVector<QPointF> gammaSpectrum(double fwhm) const;

    struct DecayDataSet {
        DecayDataSet();

        QString startEnergy;
        QString startSpin;

        QString popEnergy;
        QString popIntensity;
        QString popMultipolarity;
        QString popMixing;

        QString intEnergy;
        QString intHalfLife;
        QString intSpin;
        QString intMu;
        QString intQ;

        QString depopEnergy;
        QString depopIntensity;
        QString depopMultipolarity;
        QString depopMixing;

        QString endEnergy;
        QString endSpin;

        QString a22;
        QString a24;
        QString a42;
        QString a44;
    };

signals:
    void enabledShadow(bool enable);
    void updatedDecayData(Decay::DecayDataSet data);
    
private slots:
    void itemClicked(ClickableItem *item);

private:
    void clickedGamma(GammaTransition *g);
    void clickedEnergyLevel(EnergyLevel *e);
    void sendDecayDataUpdate();
    void alignGraphicsItems();
    double gauss(const double x, const double sigma) const;

    const QString m_name;

    Nuclide *pNuc, *dNuc;
    Type t;

    QGraphicsScene *scene;

    // graphics items
    QGraphicsLineItem *pNucVerticalArrow;
    QGraphicsSimpleTextItem *pNucHl;

    // style
    QFont stdFont, stdBoldFont, nucFont, nucIndexFont, parentHlFont, feedIntensityFont, gammaFont;
    QPen levelPen, stableLevelPen, feedArrowPen, intenseFeedArrowPen, gammaPen, intenseGammaPen;

    // highlighted items
    GammaTransition *firstSelectedGamma, *secondSelectedGamma;
    EnergyLevel *selectedEnergyLevel;

    static const double outerGammaMargin; // margin between level texts (spin, energy) and gammas
    static const double outerLevelTextMargin; // level lines extend beyond the beginning/end of the level texts by this value
    static const double maxExtraLevelDistance; // maximal additional distance between two level lines
    static const double levelToHalfLifeDistance; // distance between level line and half life text
    static const double parentNuclideLevelLineLength;
    static const double parentNuclideLevelLineExtraLength; // additional length of the decaying level
    static const double parentNuclideMinSpinEnergyDistance; // Minumal distance between spin and energy texts on parent level lines
    static const double arrowHeadLength;
    static const double arrowHeadWidth;
    static const double arrowGap;
    static const double parentNuclideToEnergyLevelsDistance;
    static const double highlightWidth;
};

#endif // DECAY_H