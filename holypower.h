#ifndef HOLYPOWER_H
#define HOLYPOWER_H

#include "power.h"

class HolyPower : public Power
{
public:
    HolyPower(Ball& p) : Power(p), rad(30.0) {};
    HolyPower(Ball& p, qreal pot) : Power(p, pot), rad(30.0) {};

    qreal radius() {return rad * potency * (maxlife - lifet);};

    bool isTouching(const Ball* b) {
        return touchedBalls.contains(b);
    }

    void setTouching(const Ball* b, bool value);

    void draw(QPainter& painter) override;
    bool resolveBallCollision(Ball& other) override;
    bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) override;
private:
    QSet<const Ball*> touchedBalls;

    qreal rad;
};

#endif // HOLYPOWER_H
