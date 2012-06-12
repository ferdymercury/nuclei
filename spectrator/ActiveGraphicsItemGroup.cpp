#include "ActiveGraphicsItemGroup.h"

#include <QPen>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include "GraphicsHighlightItem.h"
#include "GraphicsDropShadowEffect.h"

const QColor ActiveGraphicsItemGroup::shadowColor(178, 223, 150, 180);
const double ActiveGraphicsItemGroup::animationDuration = 100.0;

ActiveGraphicsItemGroup::ActiveGraphicsItemGroup(QGraphicsItem * parent)
    : QGraphicsItemGroup(parent), shadow(new GraphicsDropShadowEffect), m_shape(0), m_helper(0),
      aniHighlight(0), aniShadow(0), aniGroup(0)
{
    // prepare highlighting
    shadow->setBlurRadius(15.0);
    shadow->setOffset(QPointF(0.0, 0.0));
    shadow->setColor(shadowColor);
    shadow->setOpacity(0.0);

    setGraphicsEffect(shadow);
    setAcceptHoverEvents(true);

    aniHighlight = new QPropertyAnimation(m_helper, "opacity");
    aniHighlight->setDuration(animationDuration);
    aniHighlight->setStartValue(0.0);
    aniHighlight->setEndValue(1.0);
    aniShadow = new QPropertyAnimation(shadow, "opacity");
    aniShadow->setDuration(animationDuration);
    aniShadow->setStartValue(0.0);
    aniShadow->setEndValue(1.0);
    aniGroup = new QParallelAnimationGroup;
    aniGroup->addAnimation(aniHighlight);
    aniGroup->addAnimation(aniShadow);
}

ActiveGraphicsItemGroup::~ActiveGraphicsItemGroup()
{
    delete m_shape;
    delete aniGroup;
}

void ActiveGraphicsItemGroup::addToGroup(QGraphicsItem *item)
{
    delete m_shape;
    m_shape = 0;
    QGraphicsItemGroup::addToGroup(item);
}

void ActiveGraphicsItemGroup::addHighlightHelper(GraphicsHighlightItem *helperitem)
{
    delete m_helper;
    m_helper = helperitem;
    m_helper->setOpacity(0.0);
    m_helper->setPen(Qt::NoPen);
    m_helper->setBrush(QBrush(shadowColor));
    addToGroup(m_helper);
}

void ActiveGraphicsItemGroup::removeHighlightHelper(GraphicsHighlightItem *helperitem)
{
    m_helper = 0;
    removeFromGroup(helperitem);
}


void ActiveGraphicsItemGroup::setPos(const QPointF &pos)
{
    QGraphicsItemGroup::setPos(pos);
    // force recreation of bounding rect (workaround)
    QGraphicsItem *tmp = childItems().last();
    removeFromGroup(tmp);
    addToGroup(tmp);
    delete m_shape;
    m_shape = 0;
}

void ActiveGraphicsItemGroup::setPos(qreal x, qreal y)
{
    setPos(QPointF(x, y));
}

QPainterPath ActiveGraphicsItemGroup::shape() const
{
    if (!m_shape) {
        m_shape = new QPainterPath;
        foreach (QGraphicsItem *child, childItems()) {
            m_shape->addPath(mapFromItem(child, child->shape()));
        }
    }
    return *m_shape;
}

void ActiveGraphicsItemGroup::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    aniGroup->stop();
    setZValue(1.0);
    if (m_helper && !aniHighlight->targetObject())
        aniHighlight->setTargetObject(m_helper);
    aniGroup->setDirection(QAbstractAnimation::Forward);
    aniGroup->start();
}

void ActiveGraphicsItemGroup::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    aniGroup->stop();
    setZValue(1.0);
    if (m_helper && !aniHighlight->targetObject())
        aniHighlight->setTargetObject(m_helper);
    aniGroup->setDirection(QAbstractAnimation::Backward);
    aniGroup->start();
}
