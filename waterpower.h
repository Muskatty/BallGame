#ifndef WATERPOWER_H
#define WATERPOWER_H

#include "power.h"

#include <QSet>
#include <QPainterPath>

static constexpr qreal waterMaxLife = 0.45;
static constexpr qreal waterVelModifier = 0.25;

class WaterPower : public Power
{
public:
    WaterPower(Ball* p, qreal pot);

    qreal radius() const { return radius_ * potency_ * (1.0 - lifetime_ / maxlife_); }
    qreal innerRadius() const { return std::max(0.0, radius() - thickness_ * 0.5); }
    qreal outerRadius() const { return radius() + thickness_ * 0.5; }

    qreal halfAngle() const { return halfAngle_; }

    bool isTouching(const Ball* b) const
    {
        return touchedBalls.contains(b);
    }

    void setTouching(const Ball* b, bool value);

    void draw(QPainter& painter) const override;
    bool resolveBallCollision(Ball& other) override;
    bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) override;
    void decreaseLife(qreal dt) override;

private:
    bool containsPoint(const QPointF& p) const;
    QPainterPath arcPath() const;

private:
    QSet<const Ball*> touchedBalls;

    QPointF velocity_;
    qreal baseAngle_;
    qreal radius_;
    qreal thickness_;
    qreal halfAngle_;
};

#endif // WATERPOWER_H