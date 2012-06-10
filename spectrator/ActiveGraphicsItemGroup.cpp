#include "ActiveGraphicsItemGroup.h"

#include <QGraphicsDropShadowEffect>

ActiveGraphicsItemGroup::ActiveGraphicsItemGroup(QGraphicsItem * parent)
    : QGraphicsItemGroup(parent), shadow(new QGraphicsDropShadowEffect)
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
}

void ActiveGraphicsItemGroup::moveBy(qreal dx, qreal dy)
{
    QGraphicsItemGroup::moveBy(dx, dy);
    // force recreation of bounding rect (workaround)
    QGraphicsItem *tmp = childItems().last();
    removeFromGroup(tmp);
    addToGroup(tmp);
}

void ActiveGraphicsItemGroup::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    shadow->setEnabled(true);
}

void ActiveGraphicsItemGroup::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    shadow->setEnabled(false);
}
