#ifndef ACTIVEGRAPHICSITEMGROUP_H
#define ACTIVEGRAPHICSITEMGROUP_H

#include <QGraphicsItemGroup>

class QGraphicsDropShadowEffect;

class ActiveGraphicsItemGroup : public QGraphicsItemGroup
{
public:
    ActiveGraphicsItemGroup(QGraphicsItem *parent = 0);
    ~ActiveGraphicsItemGroup();
    void moveBy(qreal dx, qreal dy);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
    QGraphicsDropShadowEffect *shadow;
};

#endif // ACTIVEGRAPHICSITEMGROUP_H
