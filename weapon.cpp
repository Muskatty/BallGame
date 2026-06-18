#include "weapon.h"
#include <QPainterPath>

void Weapon::updatePos(const QPointF& pos) {
    targetPos = pos;
}

void Weapon::move(const qreal dt) {
    angle += velocity * dt;
    if (angle > 360.0) {
        angle -= 360.0;
    }
}

void Weapon::draw(QPainter& painter) {
    qreal centerX = targetPos.x();
    qreal centerY = targetPos.y();

    painter.save();
    painter.translate(centerX, centerY);
    painter.rotate(angle);

    QRectF rect(-width / 2.0, -length, width, length);

    painter.setBrush(Qt::blue);
    painter.drawRect(rect);

    painter.restore();
}

bool Weapon::detectCellCollision(const QRectF& cellRect) const
{
    QTransform transform;
    transform.translate(targetPos.x(), targetPos.y());
    transform.rotate(angle);

    QPolygonF weaponPoly;
    weaponPoly << QPointF(-width / 2.0, -length)
               << QPointF( width / 2.0, -length)
               << QPointF( width / 2.0, 0.0)
               << QPointF(-width / 2.0, 0.0);

    weaponPoly = transform.map(weaponPoly);

    QPainterPath weaponPath;
    weaponPath.addPolygon(weaponPoly);
    weaponPath.closeSubpath();

    QPainterPath cellPath;
    cellPath.addRect(cellRect);

    return weaponPath.intersects(cellPath);
}
