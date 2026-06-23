#include "ball.h"
#include "geometry.h"

#include <cmath>
#include <algorithm>

bool Ball::detectCellCollision(const QRectF& cell) const
{
    return weapon->detectCellCollision(cell);
}

bool Ball::resolveCellCollision(const QRectF& cell)
{
    const qreal closestX = std::clamp(cntr.x(), cell.left(), cell.right());
    const qreal closestY = std::clamp(cntr.y(), cell.top(), cell.bottom());

    const qreal dx = cntr.x() - closestX;
    const qreal dy = cntr.y() - closestY;

    const qreal dist2 = dx * dx + dy * dy;
    const qreal r2 = r * r;

    if (dist2 > r2) {
        return false;
    }

    if (cell.contains(cntr)) {
        const qreal dl = cntr.x() - cell.left();
        const qreal dr = cell.right() - cntr.x();
        const qreal dt = cntr.y() - cell.top();
        const qreal db = cell.bottom() - cntr.y();

        const qreal m = std::min({dl, dr, dt, db});

        if (m == dl) {
            cntr.setX(cell.left() - r);
            vel.setX(-std::abs(vel.x()));
        } else if (m == dr) {
            cntr.setX(cell.right() + r);
            vel.setX(std::abs(vel.x()));
        } else if (m == dt) {
            cntr.setY(cell.top() - r);
            vel.setY(-std::abs(vel.y()));
        } else {
            cntr.setY(cell.bottom() + r);
            vel.setY(std::abs(vel.y()));
        }

        return true;
    }

    qreal dist = std::sqrt(dist2);
    if (dist == 0.0) {
        vel.setX(-vel.x());
        return true;
    }

    const QPointF n = Geometry::normalized({dx, dy});

    const qreal penetration = r - dist;
    cntr += QPointF(n.x() * penetration, n.y() * penetration);

    const qreal vn = vel.x() * n.x() + vel.y() * n.y();
    vel -= QPointF(2.0 * vn * n.x(), 2.0 * vn * n.y());

    return true;
}

bool Ball::bounceOff(Ball& other)
{
    Collision::CollisionInfo coll = Collision::circleCircle(cntr, r, other.cntr, other.r);

    if (!coll.hit) {
        return false;
    }

    QPointF normal = coll.normal;
    qreal overlap = coll.overlap;

    qreal speed1 = Geometry::length(vel);
    qreal speed2 = Geometry::length(other.vel);

    cntr += QPointF(normal.x() * overlap * 0.5, normal.y() * overlap * 0.5);
    other.cntr -= QPointF(normal.x() * overlap * 0.5, normal.y() * overlap * 0.5);

    qreal v1n = vel.x() * normal.x() + vel.y() * normal.y();
    qreal v2n = other.vel.x() * normal.x() + other.vel.y() * normal.y();

    QPointF v1t(vel.x() - v1n * normal.x(), vel.y() - v1n * normal.y());
    QPointF v2t(other.vel.x() - v2n * normal.x(), other.vel.y() - v2n * normal.y());

    QPointF newV1(v1t.x() + v2n * normal.x(), v1t.y() + v2n * normal.y());
    QPointF newV2(v2t.x() + v1n * normal.x(), v2t.y() + v1n * normal.y());

    vel = Geometry::normalized(newV1);
    other.vel = Geometry::normalized(newV2);

    vel = QPointF(vel.x() * speed1, vel.y() * speed1);
    other.vel = QPointF(other.vel.x() * speed2, other.vel.y() * speed2);

    return true;
}

bool Ball::resolveUpgradeCollision(Upgrade& other) {
    Collision::CollisionInfo coll = Collision::circleCircle(cntr, r, other.pos(), other.radius());

    if (!coll.hit) {
        return false;
    }

    switch (other.type()) {
    case UpgradeType::Health:
        hp += HealthBonus;
        break;
    case UpgradeType::Speed:
        acceleratePercent(SpeedPercentBonus);
        break;
    case UpgradeType::SpinSpeed:
        weapon->accelerate(SpinSpeedBonus);
        break;
    case UpgradeType::Strength:
        weapon->dmgUp(StrengthBonus);
        break;
    case UpgradeType::WeaponSize:
        weapon->sizeUp(WeaponSizeBonus);
        break;
    }

    return true;
}

bool Ball::bounceOffWeapon(Ball& other) {
    if (this == &other) {
        return false;
    }

    Weapon* w = other.weapon.get();
    if (!w) {
        return false;
    }

    QTransform weaponTransform;
    weaponTransform.translate(w->pos().x(), w->pos().y());
    weaponTransform.rotate(w->angle());

    const QRectF weaponRect(-w->width() / 2.0, -w->length(), w->width(), w->length());

    const Collision::CollisionInfo coll =
        Collision::circleRect(cntr, r, weaponRect, weaponTransform);

    if (!coll.hit) {
        w->setTouching(this, false);
        return false;
    }

    if (coll.overlap > 0.0) {
        cntr += QPointF(coll.normal.x() * coll.overlap,
                        coll.normal.y() * coll.overlap);
    }

    const bool firstTouch = !w->isTouching(this);

    const qreal vn = vel.x() * coll.normal.x() + vel.y() * coll.normal.y();
    if (vn < 0.0) {
        vel -= QPointF(2.0 * vn * coll.normal.x(), 2.0 * vn * coll.normal.y());
    }

    if (firstTouch) {
        hp -= w->damage();
        w->invertVelocity();
    }

    w->setTouching(this, true);
    return true;
}

CollisionResult Ball::resolveFieldCollision(const QRectF& field)
{
    CollisionResult res;

    if (cntr.x() - r < field.left()) {
        cntr.setX(field.left() + r);
        vel.setX(-vel.x());
        res.hitX = true;
    } else if (cntr.x() + r > field.right()) {
        cntr.setX(field.right() - r);
        vel.setX(-vel.x());
        res.hitX = true;
    }

    if (cntr.y() - r < field.top()) {
        cntr.setY(field.top() + r);
        vel.setY(-vel.y());
        res.hitY = true;
    } else if (cntr.y() + r > field.bottom()) {
        cntr.setY(field.bottom() - r);
        vel.setY(-vel.y());
        res.hitY = true;
    }

    return res;
}

void Ball::accelerate(qreal accel) {
    vel.setX(vel.x() + accel);
    vel.setY(vel.y() + accel);
}

void Ball::acceleratePercent(qreal percent) {
    vel.setX(vel.x() * (1 + percent));
    vel.setY(vel.y() * (1 + percent));
}

void Ball::draw(QPainter& painter)
{
    weapon->draw(painter);

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(cntr, r, r);

    QRect rect(cntr.x() - r, cntr.y() - r / 2, r * 2, r);
    painter.setFont(QFont("Helvetica", 12));
    painter.setPen(Qt::black);
    painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, QString::number(hp));
}

void Ball::move(const qreal dt)
{
    cntr.setX(cntr.x() + vel.x() * dt);
    cntr.setY(cntr.y() + vel.y() * dt);
    weapon->updatePos(cntr);
    weapon->move(dt);
}