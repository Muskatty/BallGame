#include "ball.h"
#include "geometry.h"

#include <cmath>
#include <algorithm>

Ball::Ball(const BallConfig& cfg) :
    center_(cfg.initialPosition),
    radius_(cfg.radius),
    velocity_(cfg.initialVelocity),
    hp_(cfg.hp),
    power_(cfg.type),
    traceColor_(powerToColor.at(cfg.type))
{
    weapon_ = std::make_unique<Weapon>(cfg.weapon);
    weapon_->setPosition(center_);
}

bool Ball::detectCellWeaponCollision(const QRectF& cell) const
{
    return weapon_->detectCellCollision(cell);
}

bool Ball::resolveCellCollision(const QRectF& cell)
{
    const qreal closestX = std::clamp(center_.x(), cell.left(), cell.right());
    const qreal closestY = std::clamp(center_.y(), cell.top(), cell.bottom());

    const qreal dx = center_.x() - closestX;
    const qreal dy = center_.y() - closestY;

    const qreal dist2 = dx * dx + dy * dy;
    const qreal r2 = radius_ * radius_;

    if (dist2 > r2) {
        return false;
    }

    if (cell.contains(center_)) {
        const qreal dl = center_.x() - cell.left();
        const qreal dr = cell.right() - center_.x();
        const qreal dt = center_.y() - cell.top();
        const qreal db = cell.bottom() - center_.y();

        const qreal m = std::min({dl, dr, dt, db});

        if (m == dl) {
            center_.setX(cell.left() - radius_);
            velocity_.setX(-std::abs(velocity_.x()));
        } else if (m == dr) {
            center_.setX(cell.right() + radius_);
            velocity_.setX(std::abs(velocity_.x()));
        } else if (m == dt) {
            center_.setY(cell.top() - radius_);
            velocity_.setY(-std::abs(velocity_.y()));
        } else {
            center_.setY(cell.bottom() + radius_);
            velocity_.setY(std::abs(velocity_.y()));
        }

        return true;
    }

    qreal dist = std::sqrt(dist2);
    if (dist == 0.0) {
        velocity_.setX(-velocity_.x());
        return true;
    }

    const QPointF n = Geometry::normalized({dx, dy});

    const qreal penetration = radius_ - dist;
    center_ += QPointF(n.x() * penetration, n.y() * penetration);

    const qreal vn = velocity_.x() * n.x() + velocity_.y() * n.y();
    velocity_ -= QPointF(2.0 * vn * n.x(), 2.0 * vn * n.y());

    return true;
}

bool Ball::bounceOffBall(Ball& other)
{
    Collision::CollisionInfo coll = Collision::circleCircle(center_, radius_, other.center_, other.radius_);

    if (!coll.hit) {
        return false;
    }

    QPointF normal = coll.normal;
    qreal overlap = coll.overlap;

    qreal speed1 = Geometry::length(velocity_);
    qreal speed2 = Geometry::length(other.velocity_);

    center_ += QPointF(normal.x() * overlap * 0.5, normal.y() * overlap * 0.5);
    other.center_ -= QPointF(normal.x() * overlap * 0.5, normal.y() * overlap * 0.5);

    qreal v1n = velocity_.x() * normal.x() + velocity_.y() * normal.y();
    qreal v2n = other.velocity_.x() * normal.x() + other.velocity_.y() * normal.y();

    QPointF v1t(velocity_.x() - v1n * normal.x(), velocity_.y() - v1n * normal.y());
    QPointF v2t(other.velocity_.x() - v2n * normal.x(), other.velocity_.y() - v2n * normal.y());

    QPointF newV1(v1t.x() + v2n * normal.x(), v1t.y() + v2n * normal.y());
    QPointF newV2(v2t.x() + v1n * normal.x(), v2t.y() + v1n * normal.y());

    velocity_ = Geometry::normalized(newV1);
    other.velocity_ = Geometry::normalized(newV2);

    velocity_ = QPointF(velocity_.x() * speed1, velocity_.y() * speed1);
    other.velocity_ = QPointF(other.velocity_.x() * speed2, other.velocity_.y() * speed2);

    return true;
}

void Ball::applyUpgrade(UpgradeType type) {
    switch (type) {
    case UpgradeType::Health:
        hp_ += HealthBonus;
        return;
    case UpgradeType::Speed:
        acceleratePercent(SpeedPercentBonus);
        return;
    case UpgradeType::SpinSpeed:
        weapon_->accelerate(SpinSpeedBonus);
        return;
    case UpgradeType::Strength:
        weapon_->dmgUp(StrengthBonus);
        return;
    case UpgradeType::WeaponSize:
        weapon_->sizeUp(WeaponSizeBonus);
        return;
    case UpgradeType::Power:
    case UpgradeType::Count:
        return;
    }
}

bool Ball::resolveUpgradeCollision(Upgrade& other) {
    Collision::CollisionInfo coll = Collision::circleCircle(center_, radius_, other.pos(), other.radius());

    if (!coll.hit) {
        return false;
    }

    applyUpgrade(other.type());

    return true;
}

bool Ball::bounceOffWeapon(Ball& other) {
    if (this == &other) {
        return false;
    }

    Weapon* w = other.weapon_.get();
    if (!w) {
        return false;
    }

    QTransform weaponTransform;
    weaponTransform.translate(w->pos().x(), w->pos().y());
    weaponTransform.rotate(w->angle());

    const QRectF weaponRect(-w->width() / 2.0, -w->length(), w->width(), w->length());

    const Collision::CollisionInfo coll =
        Collision::circleRect(center_, radius_, weaponRect, weaponTransform);

    if (!coll.hit) {
        w->setTouching(this, false);
        return false;
    }

    if (coll.overlap > 0.0) {
        center_ += QPointF(coll.normal.x() * coll.overlap,
                        coll.normal.y() * coll.overlap);
    }

    const bool firstTouch = !w->isTouching(this);

    const qreal vn = velocity_.x() * coll.normal.x() + velocity_.y() * coll.normal.y();
    if (vn < 0.0) {
        velocity_ -= QPointF(2.0 * vn * coll.normal.x(), 2.0 * vn * coll.normal.y());
    }

    if (firstTouch) {
        hp_ -= w->damage();
        w->invertVelocity();
    }

    w->setTouching(this, true);
    return true;
}

CollisionResult Ball::resolveFieldCollision(const QRectF& field)
{
    CollisionResult res;

    if (center_.x() - radius_ < field.left()) {
        center_.setX(field.left() + radius_);
        velocity_.setX(-velocity_.x());
        res.hitX = true;
    } else if (center_.x() + radius_ > field.right()) {
        center_.setX(field.right() - radius_);
        velocity_.setX(-velocity_.x());
        res.hitX = true;
    }

    if (center_.y() - radius_ < field.top()) {
        center_.setY(field.top() + radius_);
        velocity_.setY(-velocity_.y());
        res.hitY = true;
    } else if (center_.y() + radius_ > field.bottom()) {
        center_.setY(field.bottom() - radius_);
        velocity_.setY(-velocity_.y());
        res.hitY = true;
    }

    return res;
}

void Ball::accelerate(qreal accel) {
    velocity_.setX(velocity_.x() + accel);
    velocity_.setY(velocity_.y() + accel);
}

void Ball::acceleratePercent(qreal percent) {
    velocity_.setX(velocity_.x() * (1 + percent));
    velocity_.setY(velocity_.y() * (1 + percent));
}

void Ball::draw(QPainter& painter) const
{
    weapon_->draw(painter);

    painter.setBrush(ballColor_);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center_, radius_, radius_);

    QRect rect(center_.x() - radius_, center_.y() - radius_ / 2, radius_ * 2, radius_);
    painter.setFont(QFont("Helvetica", 12));
    painter.setPen(Qt::black);
    painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, QString::number(hp_));
}

void Ball::move(const qreal dt)
{
    center_.setX(center_.x() + velocity_.x() * dt);
    center_.setY(center_.y() + velocity_.y() * dt);
    weapon_->updatePos(center_);
    weapon_->move(dt);
}