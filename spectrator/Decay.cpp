#include "Decay.h"
#include <QDebug>
#include <QLocale>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QFontMetrics>
#include <cmath>


const double Decay::outerGammaMargin = 50.0;
const double Decay::outerLevelTextMargin = 4.0; // level lines extend beyond the beginning/end of the level texts by this value
const double Decay::maxExtraLevelDistance = 120.0;
const double Decay::levelToHalfLifeDistance = 10.0;
const double Decay::parentNuclideLevelLineLength = 110.0;
const double Decay::parentNuclideLevelLineExtraLength = 15.5;
const double Decay::arrowHeadLength = 10.0;
const double Decay::arrowGap = 5.0;
const double Decay::parentNuclideToEnergyLevelsDistance = 30.0;


Decay::Decay(Nuclide parentNuclide, Nuclide daughterNuclide, Type decayType, QObject *parent)
    : QObject(parent), pNuc(parentNuclide), dNuc(daughterNuclide), t(decayType),
      parentDecayStartEnergyEv(0), normalizeDecIntensToPercentParentDecay(1.0)
{
}

/**
 * Processes a decay header from an ENSDF data file
 */
Decay::Decay(const QStringList &ensdfData, QObject *parent)
    : QObject(parent),
      parentDecayStartEnergyEv(0), normalizeDecIntensToPercentParentDecay(1.0),
      ensdf(ensdfData), scene(0)
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
    // parse information from parent record
    HalfLife hl;
    int pidx = ensdf.indexOf(QRegExp("^" + head.mid(9, 5) + "\\s\\sP\\s.*$"));
    if (pidx > 0) {
        QString prec(ensdf.at(pidx));

        // determine parent's half-life
        hl = HalfLife(prec.mid(39, 10));

        // determine decaying level
        QLocale clocale("C");
        parentDecayStartEnergyEv = uint64_t(clocale.toDouble(prec.mid(9, 10).trimmed())*1000.+0.5);

        // determine parent level's spin
        parentDecayStartSpin = SpinParity(prec.mid(21, 17));
    }
    // create parent nuclide
    Nuclide p(head.mid(9, 3).trimmed().toUInt(), head.mid(12, 2).trimmed(), hl);

    pNuc = p;
}

Decay::~Decay()
{
    foreach (EnergyLevel *level, levels)
        delete level;
    levels.clear();
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

QGraphicsScene * Decay::levelPlot()
{
    if (!scene) {
        processENSDFLevels();

        scene = new QGraphicsScene(this);

        // decide if parent nuclide should be printed on the left side (beta-),
        // on the right side (EC, beta+, alpha) or not at all (isomeric)
        enum ParentPosition {
            NoParent,
            LeftParent,
            RightParent
        } parentpos = RightParent;
        if (t == IsomericTransition)
            parentpos = NoParent;
        else if (t == BetaMinus)
            parentpos = LeftParent;

        // determine space needed for gammas
        double gammaspace = 300.0;

        // prepare fonts and their metrics
        QFont stdFont;
        QFont stdBoldFont;
        stdBoldFont.setBold(true);
        QFont nucFont;
        nucFont.setPointSizeF(nucFont.pointSizeF() * 2.5);
        nucFont.setBold(true);
        QFont nucIndexFont;
        nucIndexFont.setPointSizeF(nucIndexFont.pointSizeF() * 1.5);
        nucIndexFont.setBold(true);
        QFont parentHlFont;
        parentHlFont.setPointSizeF(parentHlFont.pointSizeF() * 1.3);
        QFont feedIntensityFont;
        feedIntensityFont.setItalic(true);

        QFontMetrics stdFontMetrics(stdFont);
        QFontMetrics stdBoldFontMetrics(stdBoldFont);
        QFontMetrics parentHlFontMetrics(parentHlFont);
        QFontMetrics feedIntensityFontMetrics(feedIntensityFont);

        // prepare pens
        QPen levelPen;
        levelPen.setWidthF(1.0);
        levelPen.setCapStyle(Qt::FlatCap);
        QPen stableLevelPen;
        stableLevelPen.setWidthF(2.0);
        stableLevelPen.setCapStyle(Qt::FlatCap);
        QPen feedArrowPen;
        feedArrowPen.setWidthF(1.0);
        feedArrowPen.setCapStyle(Qt::SquareCap);

        // create level items and determine max label widths
        double maxEnergyLabelWidth = 0.0;
        double maxSpinLabelWidth = 0.0;
        foreach (EnergyLevel *level, levels) {
            level->gragroup = new QGraphicsItemGroup;

            level->graline = new QGraphicsLineItem(-outerGammaMargin, 0.0, outerGammaMargin, 0.0, level->gragroup);
            level->graline->setPen(levelPen);
            // thick line for stable/isomeric levels
            if (level->halfLife().isStable() || level->isomerNum() > 0)
                level->graline->setPen(stableLevelPen);

            QString etext = level->energyAsText();
            level->graetext = new QGraphicsSimpleTextItem(etext, level->gragroup);
            level->graetext->setFont(stdBoldFont);
            if (stdBoldFontMetrics.width(etext) > maxEnergyLabelWidth)
                maxEnergyLabelWidth = stdBoldFontMetrics.width(etext);
            level->graetext->setPos(0.0, -stdBoldFontMetrics.height());

            QString spintext = level->spin().toString();
            level->graspintext = new QGraphicsSimpleTextItem(spintext, level->gragroup);
            level->graspintext->setFont(stdBoldFont);
            if (stdBoldFontMetrics.width(spintext) > maxSpinLabelWidth)
                maxSpinLabelWidth = stdBoldFontMetrics.width(spintext);
            level->graspintext->setPos(0.0, -stdBoldFontMetrics.height());

            QString hltext = level->halfLife().toString();
            level->grahltext = new QGraphicsSimpleTextItem(hltext, level->gragroup);
            level->grahltext->setFont(stdFont);
            level->grahltext->setPos(0.0, -0.5*stdBoldFontMetrics.height());

            level->gragroup->addToGroup(level->graline);
            level->gragroup->addToGroup(level->graetext);
            level->gragroup->addToGroup(level->graspintext);
            level->gragroup->addToGroup(level->grahltext);
            scene->addItem(level->gragroup);

            // plot feeding arrow if necessary
            double feedintensity = level->normalizedFeedIntensity();
            if (std::isfinite(feedintensity)) {
                // create line
                level->grafeedarrow = new QGraphicsLineItem;
                level->grafeedarrow->setPen(feedArrowPen);
                scene->addItem(level->grafeedarrow);
                // create arrow head
                QPolygonF arrowpol;
                arrowpol << QPointF(0.0, 0.0);
                arrowpol << QPointF((parentpos == RightParent ? 1.0 : -1.0) * arrowHeadLength, 0.2*arrowHeadLength);
                arrowpol << QPointF((parentpos == RightParent ? 1.0 : -1.0) * arrowHeadLength, -0.2*arrowHeadLength);
                level->graarrowhead = new QGraphicsPolygonItem(arrowpol);
                level->graarrowhead->setBrush(QColor(level->grafeedarrow->pen().color()));
                scene->addItem(level->graarrowhead);
                // create intensity label
                level->grafeedintens = new QGraphicsSimpleTextItem(QString("%1 %").arg(feedintensity));
                level->grafeedintens->setFont(feedIntensityFont);
                scene->addItem(level->grafeedintens);
            }
        }
        // determine y coordinates for all levels
        double maxEnergyGap = 0.0;
        QList<unsigned int> energies(levels.keys());
        for (int i=1; i<energies.size(); i++) {
            double diff = double(energies.at(i)) - double(energies.at(i-1));
            maxEnergyGap = qMax(maxEnergyGap, diff);
        }

        QList<double> yPositions;
        yPositions << 0.0;
        for (int i=1; i<energies.size(); i++) {
            double minheight = (levels.value(energies.at(i))->gragroup->boundingRect().height() + levels.value(energies.at(i-1))->gragroup->boundingRect().height())/2.0;
            double extraheight = maxExtraLevelDistance * (double(energies.at(i)) - double(energies.at(i-1))) / maxEnergyGap;
            yPositions << qRound(yPositions.at(i-1) - minheight - extraheight);
        }

        // calculate length of level lines
        double leftlinelength = outerLevelTextMargin + maxSpinLabelWidth + outerGammaMargin + gammaspace/2.0;
        double rightlinelength = outerLevelTextMargin + maxEnergyLabelWidth + outerGammaMargin + gammaspace/2.0;

        // create parent nuclide label and level(s)
        //   initialize pNucLineLength to make it updateable
        double pNucLineLength = parentNuclideLevelLineLength;
        //   create graphics items
        QGraphicsItem *pNucGra = 0;
        QGraphicsLineItem *pNucBaseLevel = 0, *pNucStartLevel = 0;
        QGraphicsLineItem *pNucVerticalArrow = 0;
        QGraphicsSimpleTextItem *pNucHl = 0, *pNucBaseEnergy = 0, *pNucEnergy = 0, *pNucSpin = 0;
        if (parentpos == LeftParent || parentpos == RightParent) {
            // create nuclide label
            pNucGra = pNuc.nuclideGraphicsItem(nucFont, nucIndexFont);
            scene->addItem(pNucGra);
            pNucLineLength = qMax(pNucLineLength, pNucGra->boundingRect().width() + 20.0);

            // create half-life label
            pNucHl = new QGraphicsSimpleTextItem(pNuc.halfLife().toString());
            pNucHl->setFont(parentHlFont);
            scene->addItem(pNucHl);

            // create vertical arrow component
            pNucVerticalArrow = new QGraphicsLineItem;
            pNucVerticalArrow->setPen(feedArrowPen);
            scene->addItem(pNucVerticalArrow);

            // parent's base level line
            pNucBaseLevel = new QGraphicsLineItem;
            pNucBaseLevel->setPen(stableLevelPen);
            scene->addItem(pNucBaseLevel);

            // parent's base level energy
            pNucBaseEnergy = new QGraphicsSimpleTextItem("0 keV");
            pNucBaseEnergy->setFont(stdBoldFont);
            scene->addItem(pNucBaseEnergy);

            // parent's start level spin
            pNucSpin = new QGraphicsSimpleTextItem(parentDecayStartSpin.toString());
            pNucSpin->setFont(stdBoldFont);
            scene->addItem(pNucSpin);

            if (parentDecayStartEnergyEv) {
                // parent's start level line
                pNucStartLevel = new QGraphicsLineItem;
                pNucStartLevel->setPen(stableLevelPen);
                scene->addItem(pNucStartLevel);

                // parent's start level energy
                pNucEnergy = new QGraphicsSimpleTextItem(QString("%1 keV").arg(double(parentDecayStartEnergyEv)/1000.0));
                pNucEnergy->setFont(stdBoldFont);
                scene->addItem(pNucEnergy);
            }
        }

        // set level item positions and sizes
        double arrowVEnd = std::numeric_limits<double>::quiet_NaN();
        for (int i=0; i<energies.size(); i++) {
            EnergyLevel *level = levels.value(energies.at(i));
            level->graline->setLine(-leftlinelength, 0.0, rightlinelength, 0.0);
            level->graspintext->setPos(-leftlinelength + outerLevelTextMargin, -stdBoldFontMetrics.height());
            level->graetext->setPos(rightlinelength - outerLevelTextMargin - stdBoldFontMetrics.width(level->graetext->text()), -stdBoldFontMetrics.height());
            double levelHlPos = 0.0;
            if (parentpos == RightParent) {
                levelHlPos = -leftlinelength - levelToHalfLifeDistance - stdFontMetrics.width(level->grahltext->text());
            }
            else {
                levelHlPos = rightlinelength + levelToHalfLifeDistance;
            }
            level->grahltext->setPos(levelHlPos, -0.5*stdBoldFontMetrics.height());
            level->gragroup->moveBy(0.0, yPositions.at(i) + 0.5*level->graline->pen().widthF()); // add 0.5*pen-width to avoid antialiasing artifacts
            if (level->grafeedarrow) {
                double leftend = (parentpos == RightParent) ? rightlinelength + arrowGap + arrowHeadLength : -leftlinelength - pNucLineLength - parentNuclideLevelLineExtraLength;
                double rightend = (parentpos == RightParent) ? rightlinelength + pNucLineLength + parentNuclideLevelLineExtraLength : -leftlinelength - arrowGap - arrowHeadLength;
                double arrowY = yPositions.at(i) + 0.5*level->grafeedarrow->pen().widthF();
                level->grafeedarrow->setLine(leftend, arrowY, rightend, arrowY);
                level->graarrowhead->setPos((parentpos == RightParent) ? rightlinelength + arrowGap : -leftlinelength - arrowGap, arrowY);
                if (std::isnan(arrowVEnd))
                    arrowVEnd = arrowY;
                level->grafeedintens->setPos(leftend + 15.0, arrowY - feedIntensityFontMetrics.height());
            }
        }

        // set position of parent nuclide
        if (parentpos == LeftParent || parentpos == RightParent) {
            double parentY = levels.value(energies.last())->gragroup->y() -
                    levels.value(energies.last())->gragroup->boundingRect().height() -
                    pNucGra->boundingRect().height() - parentNuclideToEnergyLevelsDistance;

            double parentcenter;
            if (parentpos == RightParent)
                parentcenter = rightlinelength + 0.5*(pNucLineLength+parentNuclideLevelLineExtraLength);
            else
                parentcenter = -leftlinelength - 0.5*(pNucLineLength+parentNuclideLevelLineExtraLength);

            pNucGra->setPos(parentcenter - 0.5*pNucGra->boundingRect().width(), parentY);

            // set position of parent levels
            double normalleft = (parentpos == RightParent) ? rightlinelength : -leftlinelength - pNucLineLength;
            double normalright = (parentpos == RightParent) ? rightlinelength + pNucLineLength : -leftlinelength;
            double activeleft = (parentpos == RightParent) ? normalleft : normalleft - parentNuclideLevelLineExtraLength;
            double activeright = (parentpos == RightParent) ? normalright + parentNuclideLevelLineExtraLength : normalright;

            double baseleft = 0.0;
            double baseright = 0.0;
            double startleft = 0.0;
            double startright = 0.0;

            double y = qRound(parentY - 0.3*pNucGra->boundingRect().height()) + 0.5*pNucBaseLevel->pen().widthF();

            double arrowX = (parentpos == RightParent) ? activeright : activeleft;
            double arrowVStart = y;

            double topMostLevel = y;

            if (parentDecayStartEnergyEv) {
                baseleft = normalleft;
                baseright = normalright;
                startleft = activeleft;
                startright = activeright;
                double startlevelY = y-pNucBaseEnergy->boundingRect().height() - 10.0;
                arrowVStart = startlevelY;
                topMostLevel = startlevelY;
                pNucStartLevel->setLine(startleft, startlevelY, startright, startlevelY);
                pNucEnergy->setPos(startright - stdBoldFontMetrics.width(pNucEnergy->text()), startlevelY - stdBoldFontMetrics.height());
            }
            else {
                baseleft = activeleft;
                baseright = activeright;
            }
            if (std::isfinite(arrowVEnd))
                pNucVerticalArrow->setLine(arrowX, arrowVStart, arrowX, arrowVEnd);
            pNucBaseLevel->setLine(baseleft, y, baseright, y);
            pNucBaseEnergy->setPos(baseright - stdBoldFontMetrics.width(pNucBaseEnergy->text()), y - stdBoldFontMetrics.height());
            pNucSpin->setPos(activeleft, topMostLevel - stdBoldFontMetrics.height());
            pNucHl->setPos(parentcenter - 0.5*pNucHl->boundingRect().width(), topMostLevel - stdBoldFontMetrics.height() - parentHlFontMetrics.height() - 12.0);
        }

        // create daughter nuclide label
        QGraphicsItem *dNucGra = dNuc.nuclideGraphicsItem(nucFont, nucIndexFont);
        scene->addItem(dNucGra);
        dNucGra->setPos(-0.5*dNucGra->boundingRect().width(), 0.3*dNucGra->boundingRect().height());

    }
    return scene;
}

QString Decay::toText() const
{
    QString result;
    result.append(QString("%1-%2, %3").arg(pNuc.element())
                  .arg(pNuc.a())
                  .arg(decayTypeAsText()));
    if (!pNuc.halfLife().isStable())
        result.append(", " + pNuc.halfLife().toString());
    return result;
}

void Decay::processENSDFLevels() const
{
    if (!levels.isEmpty())
        return;

    // process all level sub-blocks
    EnergyLevel *currentLevel = 0;
    QLocale clocale("C");
    bool convok;
    foreach (const QString &line, ensdf) {

        // process new level
        if (line.startsWith(dNuc.nucid() + "  L ")) {
            // determine energy
            QString estr(line.mid(9, 10));
            estr.remove('(').remove(')');
            uint64_t e = uint64_t(clocale.toDouble(estr.trimmed())*1000.+0.5);
            // determine spin
            SpinParity spin(line.mid(21, 17));
            // determine isomer number
            QString isostr(line.mid(77,2));
            unsigned int isonum = isostr.mid(1,1).toUInt(&convok);
            if (!convok && isostr.at(0) == 'M')
                isonum = 1;

            currentLevel = new EnergyLevel(e, spin, HalfLife(line.mid(39, 10)), isonum);
            levels.insert(e, currentLevel);
        }
        // process decay information
        else if (!levels.isEmpty() && line.startsWith(dNuc.nucid() + "  E ")) {
            QString iestr(line.mid(31, 7));
            iestr.remove('(').remove(')');
            double ie = clocale.toDouble(iestr.trimmed(), &convok);
            if (convok)
                currentLevel->feedintens = ie * normalizeDecIntensToPercentParentDecay;
        }
        else if (!levels.isEmpty() && line.startsWith(dNuc.nucid() + "  B ")) {
            QString ibstr(line.mid(21, 7));
            ibstr.remove('(').remove(')');
            double ib = clocale.toDouble(ibstr.trimmed(), &convok);
            if (convok)
                currentLevel->feedintens = ib * normalizeDecIntensToPercentParentDecay;
        }
        else if (!levels.isEmpty() && line.startsWith(dNuc.nucid() + "  A ")) {
            QString iastr(line.mid(21, 7));
            iastr.remove('(').remove(')');
            double ia = clocale.toDouble(iastr.trimmed(), &convok);
            if (convok)
                currentLevel->feedintens = ia * normalizeDecIntensToPercentParentDecay;
        }
        // process normalization records
        else if (line.startsWith(dNuc.nucid() + "  N ")) {
            QString brstr(line.mid(31, 7));
            brstr.remove('(').remove(')');
            double br = clocale.toDouble(brstr.trimmed(), &convok);
            if (!convok)
                br = 1.0;
            QString nbstr(line.mid(41, 7));
            nbstr.remove('(').remove(')');
            double nb = clocale.toDouble(nbstr.trimmed(), &convok);
            if (!convok)
                nb = 1.0;
            normalizeDecIntensToPercentParentDecay = nb * br;
        }
        else if (line.startsWith(dNuc.nucid() + " PN ")) {
            QString nbbrstr(line.mid(41, 7));
            nbbrstr.remove('(').remove(')');
            double nbbr = clocale.toDouble(nbbrstr.trimmed(), &convok);
            if (convok)
                normalizeDecIntensToPercentParentDecay = nbbr;
        }
    }
}

