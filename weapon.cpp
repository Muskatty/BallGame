#include "weapon.h"
#include <QPainterPath>

void Weapon::updatePos(const QPointF& pos) {
    targetPos = pos;
}

void Weapon::move(const qreal dt) {
    angl += vel * dt;
    if (angl > 360.0) {
        angl -= 360.0;
    }
}

void Weapon::draw(QPainter& painter) {
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
    QTransform transform;
    transform.translate(targetPos.x(), targetPos.y());
    transform.rotate(angl);

    QPolygonF weaponPoly;
    weaponPoly << QPointF(-w / 2.0, -l)
               << QPointF( w / 2.0, -l)
               << QPointF( w / 2.0, 0.0)
               << QPointF(-w / 2.0, 0.0);

    weaponPoly = transform.map(weaponPoly);

    QPainterPath weaponPath;
    weaponPath.addPolygon(weaponPoly);
    weaponPath.closeSubpath();

    QPainterPath cellPath;
    cellPath.addRect(cellRect);

    return weaponPath.intersects(cellPath);
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

    QPolygonF poly1;
    poly1 << rect1.topLeft()     << rect1.topRight()
          << rect1.bottomRight() << rect1.bottomLeft();

    QPolygonF poly2;
    poly2 << rect2.topLeft()     << rect2.topRight()
          << rect2.bottomRight() << rect2.bottomLeft();

    poly1 = t1.map(poly1);
    poly2 = t2.map(poly2);

    QPainterPath path1;
    path1.addPolygon(poly1);
    path1.closeSubpath();

    QPainterPath path2;
    path2.addPolygon(poly2);
    path2.closeSubpath();

    if (!path1.intersects(path2)) {
        setTouching(&other, false);
        other.setTouching(this, false);
        return false;
    }

    const bool firstTouch = !isTouching(&other);

    setTouching(&other, true);
    other.setTouching(this, true);

    if (firstTouch) {
        invertVelocity();
        other.invertVelocity();
    }

    return true;
}