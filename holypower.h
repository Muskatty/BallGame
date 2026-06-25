#ifndef HOLYPOWER_H
#define HOLYPOWER_H

#include "power.h"

static constexpr qreal holyMaxLife = 0.33;

class HolyPower : public Power
{
public:
    HolyPower(Ball& p, qreal pot) : Power(p, pot), rad(30.0) {
        maxlife = holyMaxLife;
        lifet = maxlife;
    };

    qreal radius() const {return rad * potency * (1 - (lifet / maxlife));};

    bool isTouching(const Ball* b) const {
        return touchedBalls.contains(b);
    }

    void setTouching(const Ball* b, bool value);

    void draw(QPainter& painter) const override;
    bool resolveBallCollision(Ball& other) override;
    bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) override;
private:
    QSet<const Ball*> touchedBalls;

    qreal rad;
};

#endif // HOLYPOWER_H
