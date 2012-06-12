#include "GammaTransition.h"

#include <QGraphicsTextItem>
#include <QFontMetrics>
#include <QList>
#include <QTextDocument>
#include <cmath>
#include "ActiveGraphicsItemGroup.h"
#include "EnergyLevel.h"

const double GammaTransition::textAngle = -60.0;
const double GammaTransition::arrowHeadLength = 11.0;
const double GammaTransition::arrowBaseLength = 7.0;
const double GammaTransition::arrowHeadWidth = 5.0;
const double GammaTransition::arrowBaseWidth = 5.0;
const QPolygonF GammaTransition::arrowHeadShape = initArrowHead();
const QPolygonF GammaTransition::arrowBaseShape = initArrowBase();


GammaTransition::GammaTransition(int64_t energyEV, double intensity, EnergyLevel *start, EnergyLevel *dest)
    : e(energyEV), intens(intensity), m_start(start), m_dest(dest),
      item(0), arrow(0), text(0), arrowhead(0), arrowbase(0), clickarea(0), mindist(0.0)
{
    start->m_depopulatingTransitions.append(this);
    dest->m_populatingTransitions.append(this);
}

int64_t GammaTransition::energyEv() const
{
    return e;
}

QGraphicsItem *GammaTransition::createGammaGraphicsItem(const QFont &gammaFont, const QPen &gammaPen, const QPen &intenseGammaPen)
{
    if (item)
        return item;

    m_pen = gammaPen;
    if (intens >= 5.0)
        m_pen = intenseGammaPen;

    // group origin is set to the start level!
    item = new ActiveGraphicsItemGroup;

    arrowhead = new QGraphicsPolygonItem(arrowHeadShape);
    arrowhead->setBrush(QBrush(m_pen.color()));
    arrowhead->setPen(Qt::NoPen);

    arrowbase = new QGraphicsPolygonItem(arrowBaseShape);
    arrowbase->setBrush(QBrush(m_pen.color()));
    arrowbase->setPen(Qt::NoPen);
    arrowbase->setPos(0.0, 0.0);

    arrow = new QGraphicsLineItem;
    arrow->setPen(m_pen);

    QString intensstr;
    if (!std::isnan(intens))
        intensstr = QString("%1 % ").arg(intens, 0, 'g', 3);
    QString textstr = QString("<html><body bgcolor=\"white\">%1<b>%2 keV</b></body></html>").arg(intensstr).arg(double(e)/1000.0);
    text = new QGraphicsTextItem;
    text->setFont(gammaFont);
    text->document()->setDocumentMargin(0.0);
    text->setHtml(textstr);
    //new QGraphicsRectItem(text->boundingRect(), text);
    double textheight = text->boundingRect().height();
    text->setPos(0.0, -textheight);
    text->setTransformOriginPoint(0.0, 0.5*textheight);
    text->setRotation(textAngle);
    mindist = std::abs(textheight / std::sin(-textAngle/180.0*M_PI));
    text->moveBy(0.5*textheight*std::sin(-textAngle/180.0*M_PI) - 0.5*mindist, 0.0);

    clickarea = new QGraphicsRectItem(-0.5*mindist, 0.0, mindist, 0.0);
    clickarea->setPen(Qt::NoPen);
    clickarea->setBrush(Qt::NoBrush);

    updateArrow();

    item->addToGroup(arrowbase);
    item->addToGroup(text);
    item->addToGroup(arrow);
    item->addToGroup(arrowhead);
    item->addToGroup(clickarea);

    return item;
}

QGraphicsItem * GammaTransition::gammaGraphicsItem() const
{
    return item;
}

void GammaTransition::updateArrow()
{
    item->removeFromGroup(clickarea);
    item->removeFromGroup(arrowhead);
    item->removeFromGroup(arrow);
    double arrowDestY = m_dest->graYPos - m_start->graYPos;
    arrowhead->setPos(0.0, arrowDestY);
    arrow->setLine(0.0, 0.0, 0.0, arrowDestY - arrowHeadLength);
    clickarea->setRect(-0.5*mindist, 0.0, mindist, arrowDestY);
    item->addToGroup(arrow);
    item->addToGroup(arrowhead);
    item->addToGroup(clickarea);
}

double GammaTransition::minimalXDistance() const
{
    return mindist;
}

double GammaTransition::widthFromOrigin() const
{
    return item->childrenBoundingRect().right();
}

QPen GammaTransition::pen() const
{
    return m_pen;
}

QPolygonF GammaTransition::initArrowHead()
{
    QPolygonF arrowheadpol;
    arrowheadpol << QPointF(0.0, 0.0);
    arrowheadpol << QPointF(0.5*arrowHeadWidth, -arrowHeadLength);
    arrowheadpol << QPointF(-0.5*arrowHeadWidth, -arrowHeadLength);
    return arrowheadpol;
}

QPolygonF GammaTransition::initArrowBase()
{
    QPolygonF arrowbasepol;
    arrowbasepol << QPointF(0.0, arrowBaseLength);
    arrowbasepol << QPointF(0.5*arrowBaseWidth, 0.0);
    arrowbasepol << QPointF(-0.5*arrowBaseWidth, 0.0);
    return arrowbasepol;
}
