#include "earthpower.h"
#include "geometry.h"

EarthPower::EarthPower(Ball* p, qreal pot) : Power(p, pot) {
    maxlife = earthMaxLife;
    lifet = maxlife;

    radius = earthRadiusBase * pot * earthPotencyMul;
    vel = p->velocity() * (1 + earthVelocityMul);
}

void EarthPower::draw(QPainter& painter) const {
    painter.save();
    painter.setPen(Qt::black);
    painter.setBrush(powerToColor[PowerType::Earth]);

    painter.drawEllipse(position, radius, radius);

    painter.restore();
}

bool EarthPower::resolveBallCollision(Ball& other) {
    if (!parent) {
        lifet = -1.0;
        return false;
    }

    if (parent == &other || lifet < 0.0) {
        return false;
    }

    Collision::CollisionInfo coll = Collision::circleCircle(position, radius, other.center(), other.radius());

    if (!coll.hit) {
        return false;
    }

    other.takeDamage(parent->weaponDmg() * earthDamageMul);
    lifet = -1.0;

    return true;
}

bool EarthPower::resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) {
    bool collided = false;

    if (position.x() - radius < 0.0) {
        position.setX(radius);
        vel.setX(-vel.x());
        collided = true;
    } else if (position.x() + radius > width) {
        position.setX(width - radius);
        vel.setX(-vel.x());
        collided = true;
    }

    if (position.y() - radius < 0.0) {
        position.setY(radius);
        vel.setY(-vel.y());
        collided = true;
    } else if (position.y() + radius > height) {
        position.setY(height - radius);
        vel.setY(-vel.y());
        collided = true;
    }

    if (collided) {
        accelerate(1.0 + earthVelocityMul);
    }

    return collided;
}

void EarthPower::decreaseLife(qreal dt) {
    lifet -= dt;
    update(dt);
}

void EarthPower::update(qreal dt) {
    position += vel * dt;
}
