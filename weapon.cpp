#include "weapon.h"
#include "geometry.h"
#include <QPainterPath>
#include <qdebug.h>

Weapon::Weapon(const WeaponConfig& cfg) :
    targetPos(cfg.initialPosition),
    vel(cfg.velocity),
    w(cfg.width),
    l(cfg.length),
    dmg(cfg.damage)
{

}

void Weapon::updatePos(const QPointF& pos) {
    targetPos = pos;
}

void Weapon::move(const qreal dt) {
    prevAngl = angl;
    angl += vel * dt;
    if (angl > 360.0) {
        angl -= 360.0;
    } else if (angl < 0.0) {
        angl += 360.0;
    }
}

void Weapon::draw(QPainter& painter) const {
    qreal centerX = targetPos.x();
    qreal centerY = targetPos.y();

    painter.save();
    painter.translate(centerX, centerY);
    painter.rotate(angl);

    QRectF rect(-w / 2.0, -l, w, l);

    painter.setBrush(Qt::blue);
    painter.drawRect(rect);

    painter.restore();
}

bool Weapon::detectCellCollision(const QRectF& cellRect) const
{
    const QRectF weaponRect(-w / 2.0, -l, w, l);

    QTransform weaponTransform;
    weaponTransform.translate(targetPos.x(), targetPos.y());
    weaponTransform.rotate(angl);

    const QTransform cellTransform;

    const Collision::CollisionInfo coll =
        Collision::rectRect(weaponRect, weaponTransform, cellRect, cellTransform);

    return coll.hit;
}

void Weapon::setTouching(const Ball* b, bool value) {
    if (value) {
        touchingBalls.insert(b);
    } else {
        touchingBalls.remove(b);
    }
}

void Weapon::setTouching(const Weapon* w, bool value) {
    if (value) {
        touchingWeapons.insert(w);
    } else {
        touchingWeapons.remove(w);
    }
}

bool Weapon::bounceOffWeapon(Weapon& other)
{
    if (this == &other) {
        return false;
    }

    const QRectF rect1(-width() / 2.0, -length(), width(), length());
    const QRectF rect2(-other.width() / 2.0, -other.length(), other.width(), other.length());

    QTransform t1;
    t1.translate(pos().x(), pos().y());
    t1.rotate(angle());

    QTransform t2;
    t2.translate(other.pos().x(), other.pos().y());
    t2.rotate(other.angle());

    const Collision::CollisionInfo coll = Collision::rectRect(rect1, t1, rect2, t2);

    if (!coll.hit) {
        setTouching(&other, false);
        other.setTouching(this, false);
        return false;
    }

    const bool firstTouch = !isTouching(&other);

    setTouching(&other, true);
    other.setTouching(this, true);

    if (firstTouch) {
        if (coll.overlap > 0.05) {
            angl = prevAngl;
            other.angl = other.prevAngl;
        }
        invertVelocity();
        other.invertVelocity();
    }

    return true;
}