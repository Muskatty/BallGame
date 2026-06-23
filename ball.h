#ifndef BALL_H
#define BALL_H

#include "weapon.h"
#include "upgrade.h"
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
    Ball() : cntr(0, 0), vel(0, 0), r(0) {};
    Ball(QPointF center_, QPointF velocity_, qreal radius_):
        cntr(center_),
        vel(velocity_),
        r(radius_) {
        weapon = std::make_unique<Weapon>(cntr, 270.0, 16.0, 60.0);
    };

    QPointF center() const {return cntr;};
    QPointF velocity() const {return vel;};
    qreal x() const {return cntr.x();};
    qreal y() const {return cntr.y();};
    qreal radius() const {return r;};
    int health() const {return hp;};
    qreal weaponLen() const {return weapon->length();};
    QColor traceColor() const {return tColor;};
    Weapon* getWeapon() const {return weapon.get();};

    void setCenter(const QPointF& p) {cntr = p;};
    void setVelocity(const QPointF& v) {vel = v;};
    void setTraceColor(const QColor color) {tColor = color;};
    void setColor(const QColor color) {this->color = color;};
    void setWeaponLen(qreal len) {weapon->setLen(len);};
    void accelerate(qreal accel);
    void acceleratePercent(qreal percent);

    bool detectCellCollision(const QRectF& cell) const;
    bool resolveCellCollision(const QRectF& cell);
    bool bounceOff(Ball& other);
    bool bounceOffWeapon(Ball& other);
    bool resolveUpgradeCollision(Upgrade& other);
    CollisionResult resolveFieldCollision(const QRectF& field);

    void draw(QPainter& p);
    void move(const qreal dt);

private:
    std::shared_ptr<Weapon> weapon;
    QPointF cntr;
    QPointF vel;
    qreal r;
    int hp = 100;
    QColor tColor = Qt::red;
    QColor color = Qt::white;
};

#endif // BALL_H
