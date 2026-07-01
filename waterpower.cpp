#include "waterpower.h"

#include <QPainter>
#include <QTransform>
#include "geometry.h"

namespace {

constexpr qreal EPS = 1e-9;
constexpr qreal RAD2DEG = 180.0 / 3.14159265358979323846;

qreal normalizeAngleDeg(qreal a)
{
    while (a < 0.0) a += 360.0;
    while (a >= 360.0) a -= 360.0;
    return a;
}

qreal signedAngleDiffDeg(qreal a, qreal b)
{
    qreal d = normalizeAngleDeg(a - b);
    if (d > 180.0) d -= 360.0;
    return d;
}

} // namespace


WaterPower::WaterPower(Ball* p, qreal pot)
    : Power(p, pot), radius_(48.0), thickness_(18.0), halfAngle_(60.0)
{
    maxlife_ = waterMaxLife;
    lifetime_ = maxlife_;

    const QPointF v = parent_->velocity();
    const qreal len = Geometry::length(v);

    if (len < EPS) {
        baseAngle_ = 0.0;
        velocity_ = QPointF(0.0, 0.0);
    } else {
        baseAngle_ = std::atan2(-v.y(), v.x()) * RAD2DEG;
        velocity_ = v * (1 + waterVelModifier);
    }
}

void WaterPower::setTouching(const Ball* b, bool value) {
    if (value) {
        touchedBalls.insert(b);
    } else {
        touchedBalls.remove(b);
    }
}

bool WaterPower::containsPoint(const QPointF& point) const {
    const QPointF center = position_;
    const qreal dx = point.x() - center.x();
    const qreal dy = point.y() - center.y();

    const qreal dist = std::hypot(dx, dy);
    if (dist < innerRadius() || dist > outerRadius()) {
        return false;
    }

    const qreal pointAngle = std::atan2(-dy, dx) * RAD2DEG;
    const qreal diff = signedAngleDiffDeg(pointAngle, baseAngle_);

    return std::abs(diff) <= halfAngle_;
}

QPainterPath WaterPower::arcPath() const {
    const QPointF center = position_;
    const qreal rIn = innerRadius();
    const qreal rOut = outerRadius();

    QPainterPath path;

    QRectF outerRect(center.x() - rOut, center.y() - rOut, 2.0 * rOut, 2.0 * rOut);
    QRectF innerRect(center.x() - rIn, center.y() - rIn, 2.0 * rIn, 2.0 * rIn);

    const qreal startDeg = baseAngle_ - halfAngle_;
    const qreal spanDeg = 2.0 * halfAngle_;

    path.arcMoveTo(outerRect, startDeg);
    path.arcTo(outerRect, startDeg, spanDeg);
    path.arcTo(innerRect, startDeg + spanDeg, -spanDeg);
    path.closeSubpath();

    return path;
}

void WaterPower::draw(QPainter& painter) const {
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(powerToColor[PowerType::Water]);
    painter.drawPath(arcPath());
    painter.restore();
}

bool WaterPower::resolveBallCollision(Ball& other) {
    if (parent_ == &other) {
        return false;
    }

    if (!containsPoint(other.center())) {
        setTouching(&other, false);
        return false;
    }

    const bool firstTouch = !isTouching(&other);

    if (firstTouch) {
        other.takeDamage(parent_->weaponDmg());
    }

    setTouching(&other, true);
    return true;
}

bool WaterPower::resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) {
    const QPointF center = position_;
    const qreal rOut = outerRadius();

    const int cellSize = static_cast<int>(width / field[0].size());
    const int rows = static_cast<int>(field.size());
    const int cols = static_cast<int>(field[0].size());

    const int left = std::max(0, static_cast<int>((center.x() - rOut) / cellSize));
    const int right = std::min(cols - 1, static_cast<int>((center.x() + rOut) / cellSize));
    const int top = std::max(0, static_cast<int>((center.y() - rOut) / cellSize));
    const int bottom = std::min(rows - 1, static_cast<int>((center.y() + rOut) / cellSize));

    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            const QPointF cellCenter(
                x * cellSize + cellSize * 0.5,
                y * cellSize + cellSize * 0.5
                );

            if (containsPoint(cellCenter)) {
                field[y][x] = parent_->traceColor();
            }
        }
    }

    return true;
}

void WaterPower::decreaseLife(qreal dt) {
    lifetime_ -= dt;
    position_ += velocity_ * dt;
}