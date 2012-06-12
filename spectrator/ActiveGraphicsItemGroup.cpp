#include "ActiveGraphicsItemGroup.h"

#include <QGraphicsDropShadowEffect>

ActiveGraphicsItemGroup::ActiveGraphicsItemGroup(QGraphicsItem * parent)
    : QGraphicsItemGroup(parent), shadow(new QGraphicsDropShadowEffect), m_shape(0)
{
    // prepare highlighting
    shadow->setBlurRadius(15.0);
    shadow->setOffset(QPointF(0.0, 0.0));
    shadow->setColor(QColor(232, 97, 94, 255));
    shadow->setEnabled(false);

    setGraphicsEffect(shadow);
    setAcceptHoverEvents(true);
}

ActiveGraphicsItemGroup::~ActiveGraphicsItemGroup()
{
    delete m_shape;
}

void ActiveGraphicsItemGroup::addToGroup(QGraphicsItem *item)
{
    delete m_shape;
    m_shape = 0;
    QGraphicsItemGroup::addToGroup(item);
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
    setZValue(1.0);
    shadow->setEnabled(true);
}

void ActiveGraphicsItemGroup::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setZValue(0.0);
    shadow->setEnabled(false);
}
