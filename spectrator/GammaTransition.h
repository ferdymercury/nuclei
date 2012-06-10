#ifndef GAMMATRANSITION_H
#define GAMMATRANSITION_H

#include <stdint.h>
#include <QFont>
#include <QPen>

class QGraphicsItem;
class QGraphicsItemGroup;
class QGraphicsLineItem;
class QGraphicsTextItem;
class QGraphicsPolygonItem;
class EnergyLevel;

class GammaTransition
{
public:
    GammaTransition(int64_t energyEV, double intensity, EnergyLevel *start, EnergyLevel *dest);

    int64_t energyEv() const;

    QGraphicsItem * createGammaGraphicsItem(const QFont &gammaFont, const QPen &gammaPen, const QPen &intenseGammaPen);
    QGraphicsItem * gammaGraphicsItem() const;
    double minimalXDistance() const;
    /**
     * Distance between origin and right edge of the bounding rect
     */
    double widthFromOrigin() const;

    QPen pen() const;

private:
    int64_t e;
    double intens;
    EnergyLevel *m_start, *m_dest;

    QGraphicsItemGroup *item;
    QGraphicsLineItem *arrow;
    QGraphicsTextItem *text;
    QGraphicsPolygonItem *arrowhead, *arrowbase;
    double mindist;
    QPen m_pen;

    static const double textAngle;
    static const double arrowHeadLength;
    static const double arrowBaseLength;
    static const double arrowHeadWidth;
    static const double arrowBaseWidth;

    static const QPolygonF arrowHeadShape, arrowBaseShape;
    static QPolygonF initArrowHead();
    static QPolygonF initArrowBase();
};

#endif // GAMMATRANSITION_H
