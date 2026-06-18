#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QPointF>

class Geometry
{
public:
    Geometry();
    static qreal length(const QPointF& v);
    static QPointF normalized(const QPointF& v);
};

#endif // GEOMETRY_H
