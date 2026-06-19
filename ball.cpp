#include "ball.h"
#include "geometry.h"

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
    const qreal closestX = std::clamp(center.x(), cell.left(), cell.right());
    const qreal closestY = std::clamp(center.y(), cell.top(), cell.bottom());

    const qreal dx = center.x() - closestX;
    const qreal dy = center.y() - closestY;

    const qreal dist2 = dx * dx + dy * dy;
    const qreal r2 = r * r;

    if (dist2 > r2) {
        return false;
    }

    if (cell.contains(center)) {
        const qreal dl = center.x() - cell.left();
        const qreal dr = cell.right() - center.x();
        const qreal dt = center.y() - cell.top();
        const qreal db = cell.bottom() - center.y();

        const qreal m = std::min({dl, dr, dt, db});

        if (m == dl) {
            center.setX(cell.left() - r);
            velocity.setX(-std::abs(velocity.x()));
        } else if (m == dr) {
            center.setX(cell.right() + r);
            velocity.setX(std::abs(velocity.x()));
        } else if (m == dt) {
            center.setY(cell.top() - r);
            velocity.setY(-std::abs(velocity.y()));
        } else {
            center.setY(cell.bottom() + r);
            velocity.setY(std::abs(velocity.y()));
        }

        return true;
    }

    qreal dist = std::sqrt(dist2);
    if (dist == 0.0) {
        velocity.setX(-velocity.x());
        return true;
    }

    const QPointF n = Geometry::normalized({dx, dy});

    const qreal penetration = r - dist;
    center += QPointF(n.x() * penetration, n.y() * penetration);

    const qreal vn = velocity.x() * n.x() + velocity.y() * n.y();
    velocity -= QPointF(2.0 * vn * n.x(), 2.0 * vn * n.y());

    return true;
}

bool Ball::bounceOff(Ball& other)
{
    QPointF delta = center - other.center;

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

    qreal speed1 = Geometry::length(velocity);
    qreal speed2 = Geometry::length(other.velocity);

    qreal overlap = minDist - dist;
    center += QPointF(normal.x() * overlap * 0.5, normal.y() * overlap * 0.5);
    other.center -= QPointF(normal.x() * overlap * 0.5, normal.y() * overlap * 0.5);

    qreal v1n = velocity.x() * normal.x() + velocity.y() * normal.y();
    qreal v2n = other.velocity.x() * normal.x() + other.velocity.y() * normal.y();

    QPointF v1t(velocity.x() - v1n * normal.x(), velocity.y() - v1n * normal.y());
    QPointF v2t(other.velocity.x() - v2n * normal.x(), other.velocity.y() - v2n * normal.y());

    QPointF newV1(v1t.x() + v2n * normal.x(), v1t.y() + v2n * normal.y());
    QPointF newV2(v2t.x() + v1n * normal.x(), v2t.y() + v1n * normal.y());

    velocity = Geometry::normalized(newV1);
    other.velocity = Geometry::normalized(newV2);

    velocity = QPointF(velocity.x() * speed1, velocity.y() * speed1);
    other.velocity = QPointF(other.velocity.x() * speed2, other.velocity.y() * speed2);

    return true;
}

CollisionResult Ball::resolveCollision(const QRectF& field)
{
    CollisionResult res;

    if (center.x() - r < field.left()) {
        center.setX(field.left() + r);
        velocity.setX(-velocity.x());
        res.hitX = true;
    } else if (center.x() + r > field.right()) {
        center.setX(field.right() - r);
        velocity.setX(-velocity.x());
        res.hitX = true;
    }

    if (center.y() - r < field.top()) {
        center.setY(field.top() + r);
        velocity.setY(-velocity.y());
        res.hitY = true;
    } else if (center.y() + r > field.bottom()) {
        center.setY(field.bottom() - r);
        velocity.setY(-velocity.y());
        res.hitY = true;
    }

    return res;
}

void Ball::accelerate(qreal accel) {
    velocity.setX(velocity.x() + accel);
    velocity.setY(velocity.y() + accel);
}

void Ball::draw(QPainter& painter)
{
    weapon->draw(painter);

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, r, r);
}

void Ball::move(const qreal dt)
{
    center.setX(center.x() + velocity.x() * dt);
    center.setY(center.y() + velocity.y() * dt);
    weapon->updatePos(center);
    weapon->move(dt);
}