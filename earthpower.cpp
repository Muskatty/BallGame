#include "earthpower.h"
#include "geometry.h"

EarthPower::EarthPower(Ball* p, qreal pot) : Power(p, pot) {
    maxlife_ = earthMaxLife;
    lifetime_ = maxlife_;

    radius = earthRadiusBase * pot * earthPotencyMul;
    velocity_ = p->velocity() * (1 + earthVelocityMul);
}

void EarthPower::draw(QPainter& painter) const {
    painter.save();
    painter.setPen(Qt::black);
    painter.setBrush(powerToColor[PowerType::Earth]);

    painter.drawEllipse(position_, radius, radius);

    painter.restore();
}

bool EarthPower::resolveBallCollision(Ball& other) {
    if (!parent_) {
        lifetime_ = -1.0;
        return false;
    }

    if (parent_ == &other || lifetime_ < 0.0) {
        return false;
    }

    Collision::CollisionInfo coll = Collision::circleCircle(position_, radius, other.center(), other.radius());

    if (!coll.hit) {
        return false;
    }

    other.takeDamage(parent_->weaponDmg() * earthDamageMul);
    lifetime_ = -1.0;

    return true;
}

bool EarthPower::resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) {
    bool collided = false;

    if (position_.x() - radius < 0.0) {
        position_.setX(radius);
        velocity_.setX(-velocity_.x());
        collided = true;
    } else if (position_.x() + radius > width) {
        position_.setX(width - radius);
        velocity_.setX(-velocity_.x());
        collided = true;
    }

    if (position_.y() - radius < 0.0) {
        position_.setY(radius);
        velocity_.setY(-velocity_.y());
        collided = true;
    } else if (position_.y() + radius > height) {
        position_.setY(height - radius);
        velocity_.setY(-velocity_.y());
        collided = true;
    }

    if (collided) {
        accelerate(1.0 + earthVelocityMul);
    }

    return collided;
}

void EarthPower::decreaseLife(qreal dt) {
    lifetime_ -= dt;
    update(dt);
}

void EarthPower::update(qreal dt) {
    position_ += velocity_ * dt;
}
