#ifndef HOLYPOWER_H
#define HOLYPOWER_H

#include "power.h"

static constexpr qreal holyMaxLife = 0.33;
static constexpr qreal holyBaseRadius = 30.0;

class HolyPower : public Power
{
public:
    HolyPower(Ball* p, qreal pot) : Power(p, pot), radius_(holyBaseRadius) {
        maxlife_ = holyMaxLife;
        lifetime_ = maxlife_;
    };

    qreal radius() const {return radius_ * potency_ * (1 - (lifetime_ / maxlife_));};

    bool isTouching(const Ball* b) const {
        return touchedBalls.contains(b);
    }

    void setTouching(const Ball* b, bool value);

    void draw(QPainter& painter) const override;
    bool resolveBallCollision(Ball& other) override;
    bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) override;
private:
    QSet<const Ball*> touchedBalls;

    qreal radius_;
};

#endif // HOLYPOWER_H
