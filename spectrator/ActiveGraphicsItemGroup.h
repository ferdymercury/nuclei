#ifndef ACTIVEGRAPHICSITEMGROUP_H
#define ACTIVEGRAPHICSITEMGROUP_H

#include <QGraphicsItemGroup>
#include <QColor>

class GraphicsDropShadowEffect;
class GraphicsHighlightItem;
class QPropertyAnimation;
class QParallelAnimationGroup;

class ActiveGraphicsItemGroup : public QGraphicsItemGroup
{
public:
    ActiveGraphicsItemGroup(QGraphicsItem *parent = 0);
    ~ActiveGraphicsItemGroup();
    void addToGroup(QGraphicsItem * item);
    void addHighlightHelper(GraphicsHighlightItem *helperitem);
    void removeHighlightHelper(GraphicsHighlightItem *helperitem);
    void setPos(const QPointF & pos);
    void setPos(qreal x, qreal y);
    virtual QPainterPath shape() const;

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
    GraphicsDropShadowEffect *shadow;
    mutable QPainterPath *m_shape;
    mutable GraphicsHighlightItem *m_helper;

    QPropertyAnimation *aniHighlight, *aniShadow;
    QParallelAnimationGroup *aniGroup;

    static const QColor shadowColor;
    static const double animationDuration;
    static const bool animateShadow;
    static const bool animate;
};

#endif // ACTIVEGRAPHICSITEMGROUP_H
