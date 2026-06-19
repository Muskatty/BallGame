#include "ball.h"
#include "geometry.h"

#include <cmath>
#include <algorithm>

bool Ball::detectCellCollision(const QRectF& cell) const
{
    // qreal x = center.x();
    // qreal y = center.y();

    // //is center inside
    // if (x >= leftUpper.x() && x <= rightLower.x()
    //     && y <= leftUpper.y() && y >= rightLower.y()) {
    //     return true;
    // }

    // qreal distanceX = std::min(std::abs(x - leftUpper.x()), std::abs(x - rightLower.x()));
    // qreal distanceY = std::min(std::abs(y - leftUpper.y()), std::abs(y - rightLower.y()));

    // if (distanceX * distanceX + distanceY * distanceY < r * r) {
    //     return true;
    // }

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
    QPointF delta = cntr - other.cntr;

    qreal dist2 = delta.x() * delta.x() + delta.y() * delta.y();
    qreal minDist = r + other.r;

    if (dist2 > minDist * minDist) {
        return false;
    }

    qreal dist = std::sqrt(dist2);

    if (dist == 0.0) {
        delta = QPointF(1.0, 0.0);
        dist = 1.0;
    }

    QPointF normal(delta.x() / dist, delta.y() / dist);

    qreal speed1 = Geometry::length(vel);
    qreal speed2 = Geometry::length(other.vel);

    qreal overlap = minDist - dist;
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

bool Ball::bounceOffWeapon(Ball& other) {
    if (this == &other) {
        return false;
    }

    constexpr qreal EPS = 1e-9;
    Weapon *w = other.weapon.get();

    QTransform toWorld;
    toWorld.translate(w->pos().x(), w->pos().y());
    toWorld.rotate(w->angle());

    bool invertible = false;
    QTransform toLocal = toWorld.inverted(&invertible);
    if (!invertible) {
        return false;
    }

    const QRectF rect(-w->width() / 2.0, -w->length(), w->width(), w->length());
    const QPointF c = toLocal.map(cntr);

    const QPointF closest(
        std::clamp(c.x(), rect.left(), rect.right()),
        std::clamp(c.y(), rect.top(), rect.bottom()));

    QPointF delta(c.x() - closest.x(), c.y() - closest.y());
    qreal dist2 = delta.x() * delta.x() + delta.y() * delta.y();

    if (dist2 > r * r) {
        w->setTouching(this, false);
        return false;
    }

    QPointF normalLocal;

    if (dist2 > EPS) {
        normalLocal = Geometry::normalized(delta);
    } else {
        const qreal dl = std::abs(c.x() - rect.left());
        const qreal dr = std::abs(rect.right() - c.x());
        const qreal dt = std::abs(c.y() - rect.top());
        const qreal db = std::abs(rect.bottom() - c.y());

        const qreal m = std::min({dl, dr, dt, db});

        if (m == dl)      normalLocal = QPointF(-1.0, 0.0);
        else if (m == dr) normalLocal = QPointF( 1.0, 0.0);
        else if (m == dt) normalLocal = QPointF(0.0, -1.0);
        else              normalLocal = QPointF(0.0,  1.0);
    }

    QTransform rot;
    rot.rotate(w->angle());
    QPointF normalWorld = rot.map(normalLocal);

    qreal nLen = std::hypot(normalWorld.x(), normalWorld.y());
    if (nLen > EPS) {
        normalWorld = QPointF(normalWorld.x() / nLen, normalWorld.y() / nLen);
    }

    qreal dist = std::sqrt(std::max(dist2, EPS));
    qreal penetration = r - dist;
    if (penetration > 0.0) {
        cntr += QPointF(normalWorld.x() * penetration, normalWorld.y() * penetration);
    }

    const bool firstTouch = !w->isTouching(this);

    qreal vn = vel.x() * normalWorld.x() + vel.y() * normalWorld.y();

    if (vn < 0.0) {
        vel -= QPointF(2.0 * vn * normalWorld.x(), 2.0 * vn * normalWorld.y());
    }

    if (firstTouch) {
        hp -= w->damage();
        w->invertVelocity();
    }

    w->setTouching(this, true);
    return true;
}

CollisionResult Ball::resolveCollision(const QRectF& field)
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