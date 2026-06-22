#include "geometry.h"

Geometry::Geometry() {}

qreal Geometry::length(const QPointF& v)
{
    return std::hypot(v.x(), v.y());
}

QPointF Geometry::normalized(const QPointF& v)
{
    qreal len = length(v);
    if (len == 0.0)
        return QPointF(0.0, 0.0);

    return QPointF(v.x() / len, v.y() / len);
}

namespace {

qreal dot(const QPointF& a, const QPointF& b)
{
    return a.x() * b.x() + a.y() * b.y();
}

QPointF normalizeDirection(const QTransform& transform, const QPointF& localDir)
{
    const QPointF origin = transform.map(QPointF(0.0, 0.0));
    QPointF worldDir = transform.map(localDir) - origin;

    const qreal len = std::hypot(worldDir.x(), worldDir.y());
    if (len == 0.0) {
        return QPointF(0.0, 0.0);
    }

    return QPointF(worldDir.x() / len, worldDir.y() / len);
}

QPolygonF toWorldPolygon(const QRectF& rect, const QTransform& transform)
{
    QPolygonF poly;
    poly << rect.topLeft()
         << rect.topRight()
         << rect.bottomRight()
         << rect.bottomLeft();
    return transform.map(poly);
}

void projectPolygon(const QPolygonF& poly, const QPointF& axis, qreal& minProj, qreal& maxProj)
{
    minProj = maxProj = dot(poly[0], axis);

    for (int i = 1; i < poly.size(); ++i) {
        const qreal p = dot(poly[i], axis);
        minProj = std::min(minProj, p);
        maxProj = std::max(maxProj, p);
    }
}
}

Collision::CollisionInfo Collision::circleCircle(const QPointF& center1, qreal r1,
                                                 const QPointF& center2, qreal r2) {
    QPointF delta = center1 - center2;

    qreal dist2 = delta.x() * delta.x() + delta.y() * delta.y();
    qreal minDist = r1 + r2;

    if (dist2 > minDist * minDist) {
        return Collision::NoCollision;
    }

    qreal dist = std::sqrt(dist2);
    if (dist == 0.0) {
        delta = QPointF(1.0, 0.0);
        dist = 1.0;
    }

    QPointF normal(delta.x() / dist, delta.y() / dist);
    qreal overlap = minDist - dist;

    return {true, normal, overlap};
}

Collision::CollisionInfo Collision::circleRect(const QPointF& center, qreal r,
                                               const QRectF& rect, const QTransform& transform){
    bool invertible = false;
    const QTransform toLocal = transform.inverted(&invertible);
    if (!invertible) {
        return Collision::NoCollision;
    }

    const QPointF c = toLocal.map(center);

    const bool inside =
        c.x() >= rect.left() && c.x() <= rect.right() &&
        c.y() >= rect.top()  && c.y() <= rect.bottom();

    QPointF normalLocal;
    qreal overlap = 0.0;

    if (inside) {
        const qreal dl = c.x() - rect.left();
        const qreal dr = rect.right() - c.x();
        const qreal dt = c.y() - rect.top();
        const qreal db = rect.bottom() - c.y();

        const qreal m = std::min({dl, dr, dt, db});

        if (m == dl)      normalLocal = QPointF(-1.0, 0.0);
        else if (m == dr) normalLocal = QPointF( 1.0, 0.0);
        else if (m == dt) normalLocal = QPointF(0.0, -1.0);
        else              normalLocal = QPointF(0.0,  1.0);

        overlap = r + m;
    } else {
        const QPointF closest(
            std::clamp(c.x(), rect.left(), rect.right()),
            std::clamp(c.y(), rect.top(), rect.bottom())
            );

        const QPointF delta = c - closest;
        const qreal dist2 = delta.x() * delta.x() + delta.y() * delta.y();

        if (dist2 > r * r) {
            return Collision::NoCollision;
        }

        if (dist2 > 1e-9) {
            const qreal dist = std::sqrt(dist2);
            normalLocal = QPointF(delta.x() / dist, delta.y() / dist);
            overlap = r - dist;
        } else {
            const qreal dl = c.x() - rect.left();
            const qreal dr = rect.right() - c.x();
            const qreal dt = c.y() - rect.top();
            const qreal db = rect.bottom() - c.y();

            const qreal m = std::min({dl, dr, dt, db});

            if (m == dl)      normalLocal = QPointF(-1.0, 0.0);
            else if (m == dr) normalLocal = QPointF( 1.0, 0.0);
            else if (m == dt) normalLocal = QPointF(0.0, -1.0);
            else              normalLocal = QPointF(0.0,  1.0);

            overlap = r + m;
        }
    }

    const QPointF normalWorld = normalizeDirection(transform, normalLocal);
    return {true, normalWorld, overlap};
}

Collision::CollisionInfo Collision::rectRect(const QRectF& a, const QTransform& ta,
                                             const QRectF& b, const QTransform& tb) {
    const QPolygonF polyA = toWorldPolygon(a, ta);
    const QPolygonF polyB = toWorldPolygon(b, tb);

    const QPointF centerA = ta.map(a.center());
    const QPointF centerB = tb.map(b.center());

    qreal bestOverlap = std::numeric_limits<qreal>::max();
    QPointF bestAxis;

    auto testAxis = [&](const QPointF& edge) -> bool {
        QPointF axis(-edge.y(), edge.x());
        axis = Geometry::normalized(axis);

        if (axis.x() == 0.0 && axis.y() == 0.0) {
            return true;
        }

        qreal minA, maxA, minB, maxB;
        projectPolygon(polyA, axis, minA, maxA);
        projectPolygon(polyB, axis, minB, maxB);

        if (maxA < minB || maxB < minA) {
            return false;
        }

        const qreal overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < bestOverlap) {
            bestOverlap = overlap;
            bestAxis = axis;
        }

        return true;
    };

    auto testPolygonEdges = [&](const QPolygonF& poly) -> bool {
        for (int i = 0; i < poly.size(); ++i) {
            const QPointF p1 = poly[i];
            const QPointF p2 = poly[(i + 1) % poly.size()];
            const QPointF edge = p2 - p1;

            if (!testAxis(edge)) {
                return false;
            }
        }
        return true;
    };

    if (!testPolygonEdges(polyA) || !testPolygonEdges(polyB)) {
        return Collision::NoCollision;
    }

    const QPointF centerDelta = centerB - centerA;
    if (dot(centerDelta, bestAxis) < 0.0) {
        bestAxis = QPointF(-bestAxis.x(), -bestAxis.y());
    }

    return {true, bestAxis, bestOverlap};
}