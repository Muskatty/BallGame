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