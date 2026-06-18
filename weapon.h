#ifndef WEAPON_H
#define WEAPON_H

#include <qtypes.h>
#include <QPainter>

class Weapon
{
public:
    Weapon() : targetPos(0.0, 0.0) {};
    Weapon(QPointF pos, qreal vel, qreal w, qreal l) : targetPos(pos), velocity(vel), width(w), length(l) {};

    qreal len() const {return length;};

    void setLen(qreal len) {length = len;};

    void updatePos(const QPointF& pos);
    void move(const qreal dt);
    void draw(QPainter& painter);
    bool detectCellCollision(const QRectF& cellRect) const;
private:
    QPointF targetPos;
    qreal velocity = 180.0;
    qreal width = 4.0;
    qreal length = 7.0;
    qreal angle = 0.0;
};

#endif // WEAPON_H
