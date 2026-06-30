#ifndef BALL_H
#define BALL_H

#include "weapon.h"
#include "upgrade.h"
#include <QPointF>
#include <QRectF>
#include <QPainter>
#include "powercolors.h"
#include "gameconfig.h"

struct CollisionResult {
    bool hitX = false;
    bool hitY = false;
};

class Ball
{
public:
    Ball(QPointF center, QPointF velocity, qreal radius, PowerType type):
        center_(center),
        velocity_(velocity),
        radius_(radius),
        power_(type),
        traceColor_(powerToColor.at(type))
    {
        weapon_ = std::make_unique<Weapon>(center_, 270.0, 16.0, 60.0);
    };
    Ball(const BallConfig& cfg);

    QPointF center() const {return center_;};
    QPointF velocity() const {return velocity_;};
    qreal x() const {return center_.x();};
    qreal y() const {return center_.y();};
    qreal radius() const {return radius_;};
    int health() const {return hp_;};
    qreal weaponLen() const {return weapon_->length();};
    int weaponDmg() const {return weapon_->damage();};
    PowerType powerType() const {return power_;};
    QColor traceColor() const {return traceColor_;};
    Weapon* getWeapon() const {return weapon_.get();};

    void setCenter(const QPointF& p) {center_ = p;};
    void setVelocity(const QPointF& v) {velocity_ = v;};
    void setTraceColor(const QColor color) {traceColor_ = color;};
    void setColor(const QColor color) {ballColor_ = color;};
    void setWeaponLen(qreal len) {weapon_->setLen(len);};
    void setPowerType(PowerType type) {power_ = type;};
    void accelerate(qreal accel);
    void acceleratePercent(qreal percent);
    void takeDamage(int dmg) {hp_ -= dmg;};
    void applyUpgrade(UpgradeType type);

    bool detectCellWeaponCollision(const QRectF& cell) const;
    bool resolveCellCollision(const QRectF& cell);
    bool bounceOffBall(Ball& other);
    bool bounceOffWeapon(Ball& other);
    bool resolveUpgradeCollision(Upgrade& other);
    CollisionResult resolveFieldCollision(const QRectF& field);

    void draw(QPainter& p) const;
    void move(const qreal dt);

private:
    std::unique_ptr<Weapon> weapon_;
    QPointF center_;
    QPointF velocity_;
    qreal radius_;
    int hp_ = 100;
    QColor traceColor_ = Qt::red;
    QColor ballColor_ = Qt::white;

    PowerType power_;
};

#endif // BALL_H
