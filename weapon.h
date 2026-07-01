#ifndef WEAPON_H
#define WEAPON_H

#include "gameconfig.h"
#include <qtypes.h>
#include <QPainter>
#include <QSet>

static constexpr qreal weaponWidthGrowthMultiplier = 0.125;

class Ball;

class Weapon
{
public:
    Weapon() : targetPos(0.0, 0.0) {};
    Weapon(QPointF pos, qreal vel, qreal width, qreal length) : targetPos(pos), velocity_(vel), width_(width), length_(length) {};
    Weapon(const WeaponConfig& cfg);

    QPointF pos() const {return targetPos;};
    qreal length() const {return length_;};
    qreal width() const {return width_;};
    qreal angle() const {return currentAngle_;};
    qreal velocity() const {return velocity_;};
    qreal damage() const {return dmg_;};
    bool isTouching(const Ball* b) const {
        return touchingBalls.contains(b);
    }
    bool isTouching(const Weapon* w) const {
        return touchingWeapons.contains(w);
    }


    void invertVelocity() {velocity_ = -velocity_;};
    void setPosition(QPointF pos) {targetPos = pos;};
    void setLen(qreal len) {length_ = len;};
    void setTouching(const Ball* b, bool value);
    void setTouching(const Weapon* b, bool value);

    void accelerate(qreal accel) {velocity_ = velocity_ >= 0 ? velocity_ + accel : velocity_ - accel;};
    void dmgUp(int up) {dmg_ += up;};
    void sizeUp(qreal szUp) {width_ += szUp * weaponWidthGrowthMultiplier; length_ += szUp;};

    void updatePos(const QPointF& pos);
    void move(const qreal dt);
    void draw(QPainter& painter) const;
    bool detectCellCollision(const QRectF& cellRect) const;
    bool bounceOffWeapon(Weapon& other);
private:
    QPointF targetPos;
    qreal velocity_ = 180.0;
    qreal width_ = 4.0;
    qreal length_ = 7.0;
    qreal previousAngle_ = 0.0;
    qreal currentAngle_ = 0.0;
    int dmg_ = 10;
    QSet<const Ball*> touchingBalls;
    QSet<const Weapon*> touchingWeapons;
};

#endif // WEAPON_H
