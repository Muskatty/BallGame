#ifndef BALL_H
#define BALL_H

#include "weapon.h"
#include <QPointF>
#include <QRectF>
#include <QPainter>

struct CollisionResult {
    bool hitX = false;
    bool hitY = false;
};

class Ball
{
public:
    Ball() : center(0, 0), velocity(0, 0), r(0) {};
    Ball(QPointF center_, QPointF velocity_, qreal radius_):
        center(center_),
        velocity(velocity_),
        r(radius_) {
        weapon = std::make_unique<Weapon>(center, 180.0, 16.0, 60.0);
    };

    qreal x() const {return center.x();};
    qreal y() const {return center.y();};
    qreal radius() const {return r;};
    qreal weaponLen() const {return weapon->len();};
    QColor traceColor() const {return tColor;};

    void setTraceColor(const QColor color) {tColor = color;};
    void setColor(const QColor color) {this->color = color;};
    void setWeaponLen(qreal len) {weapon->setLen(len);};
    void accelerate(qreal accel);

    bool detectCellCollision(const QPointF& leftUpper, const QPointF& rightLower) const;
    bool bounceOff(Ball& other);
    CollisionResult resolveCollision(const QRectF& field);

    void draw(QPainter& p);
    void move(const qreal dt);

private:
    std::shared_ptr<Weapon> weapon;
    QPointF center;
    QPointF velocity;
    qreal r;
    QColor tColor = Qt::red;
    QColor color = Qt::white;
};

#endif // BALL_H
