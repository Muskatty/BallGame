#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QPointF>
#include <QRectF>
#include <QTransform>

class Geometry
{
public:
    Geometry();
    static qreal length(const QPointF& v);
    static QPointF normalized(const QPointF& v);
};

namespace Collision {
    struct CollisionInfo {
        bool hit = false;
        QPointF normal;
        qreal overlap = 0.0;
    };

    constexpr CollisionInfo NoCollision = {false, QPointF(), 0.0};

    CollisionInfo circleCircle(const QPointF& center1, qreal r1,
                               const QPointF& center2, qreal r2);

    CollisionInfo circleRect(const QPointF& center, qreal r,
                             const QRectF& rect, const QTransform& transform);

    CollisionInfo rectRect(const QRectF& a, const QTransform& ta,
                           const QRectF& b, const QTransform& tb);

}

#endif // GEOMETRY_H
