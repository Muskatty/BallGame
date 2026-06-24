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

enum class PowerType;

class Ball
{
public:
    Ball(QPointF center_, QPointF velocity_, qreal radius_, PowerType type):
        cntr(center_),
        vel(velocity_),
        r(radius_),
        pwr(type)
    {
        weapon = std::make_shared<Weapon>(cntr, 270.0, 16.0, 60.0);
    };

    QPointF center() const {return cntr;};
    QPointF velocity() const {return vel;};
    qreal x() const {return cntr.x();};
    qreal y() const {return cntr.y();};
    qreal radius() const {return r;};
    int health() const {return hp;};
    qreal weaponLen() const {return weapon->length();};
    int weaponDmg() const {return weapon->damage();};
    PowerType powerType() const {return pwr;};
    QColor traceColor() const {return tColor;};
    Weapon* getWeapon() const {return weapon.get();};

    void setCenter(const QPointF& p) {cntr = p;};
    void setVelocity(const QPointF& v) {vel = v;};
    void setTraceColor(const QColor color) {tColor = color;};
    void setColor(const QColor color) {this->color = color;};
    void setWeaponLen(qreal len) {weapon->setLen(len);};
    void setPowerType(PowerType type) {pwr = type;};
    void accelerate(qreal accel);
    void acceleratePercent(qreal percent);
    void takeDamage(int dmg) {hp -= dmg;};
    void applyUpgrade(UpgradeType type);

    bool detectCellWeaponCollision(const QRectF& cell) const;
    bool resolveCellCollision(const QRectF& cell);
    bool bounceOffBall(Ball& other);
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

    PowerType pwr;
};

#endif // BALL_H
