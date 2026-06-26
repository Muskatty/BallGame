#ifndef EARTHPOWER_H
#define EARTHPOWER_H

#include "power.h"

static constexpr qreal earthMaxLife = 30.0;
static constexpr qreal earthVelocityMul = 0.10;
static constexpr qreal earthRadiusBase = 30.0;
static constexpr qreal earthPotencyMul = 0.2;
static constexpr qreal earthDamageMul = 0.5;

class EarthPower : public Power
{
public:
    EarthPower(Ball* p, qreal pot);

    void draw(QPainter& painter) const override;

    bool resolveBallCollision(Ball& other) override;
    bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) override;
    void decreaseLife(qreal dt) override;

private:
    void update(qreal dt);
    void accelerate(qreal mul) {vel *= mul;};

private:
    qreal radius = earthRadiusBase;
};

#endif // EARTHPOWER_H
