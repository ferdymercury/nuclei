#ifndef ACTIVEGRAPHICSITEMGROUP_H
#define ACTIVEGRAPHICSITEMGROUP_H

#include <QGraphicsItemGroup>

class QGraphicsDropShadowEffect;

class ActiveGraphicsItemGroup : public QGraphicsItemGroup
{
public:
    ActiveGraphicsItemGroup(QGraphicsItem *parent = 0);
    ~ActiveGraphicsItemGroup();
    void setPos(const QPointF & pos);
    virtual QPainterPath shape() const;

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
    QGraphicsDropShadowEffect *shadow;
    mutable QPainterPath *m_shape;
};

#endif // ACTIVEGRAPHICSITEMGROUP_H
