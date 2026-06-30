#ifndef WEAPON_H
#define WEAPON_H

#include "gameconfig.h"
#include <qtypes.h>
#include <QPainter>
#include <QSet>

class Ball;

class Weapon
{
public:
    Weapon() : targetPos(0.0, 0.0) {};
    Weapon(QPointF pos, qreal v, qreal wid, qreal len) : targetPos(pos), vel(v), w(wid), l(len) {};
    Weapon(const WeaponConfig& cfg);

    QPointF pos() const {return targetPos;};
    qreal length() const {return l;};
    qreal width() const {return w;};
    qreal angle() const {return angl;};
    qreal velocity() const {return vel;};
    qreal damage() const {return dmg;};
    bool isTouching(const Ball* b) const {
        return touchingBalls.contains(b);
    }
    bool isTouching(const Weapon* w) const {
        return touchingWeapons.contains(w);
    }


    void invertVelocity() {vel = -vel;};
    void setPosition(QPointF pos) {targetPos = pos;};
    void setLen(qreal len) {l = len;};
    void setTouching(const Ball* b, bool value);
    void setTouching(const Weapon* b, bool value);

    void accelerate(qreal accel) {vel = vel >= 0 ? vel + accel : vel - accel;};
    void dmgUp(int up) {dmg += up;};
    void sizeUp(qreal szUp) {w += szUp * 0.125; l += szUp;};

    void updatePos(const QPointF& pos);
    void move(const qreal dt);
    void draw(QPainter& painter) const;
    bool detectCellCollision(const QRectF& cellRect) const;
    bool bounceOffWeapon(Weapon& other);
private:
    QPointF targetPos;
    qreal vel = 180.0;
    qreal w = 4.0;
    qreal l = 7.0;
    qreal prevAngl = 0.0;
    qreal angl = 0.0;
    int dmg = 10;
    QSet<const Ball*> touchingBalls;
    QSet<const Weapon*> touchingWeapons;
};

#endif // WEAPON_H
