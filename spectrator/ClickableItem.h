#ifndef CLICKABLEITEM_H
#define CLICKABLEITEM_H

class ActiveGraphicsItemGroup;
class QGraphicsItem;

class ClickableItem
{
public:
    enum Type {
        EnergyLevelType,
        GammaTransitionType
    };

    ClickableItem(Type type);
    Type type() const;
    ActiveGraphicsItemGroup * graphicsItem() const;

protected:
    ActiveGraphicsItemGroup *item;

private:
    Type t;
};

#endif // CLICKABLEITEM_H
