#ifndef POWER_H
#define POWER_H

#include <QPolygonF>
#include <QPainter>
#include "ball.h"
#include "gamecontext.h"

class Power
{
public:
    Power(Ball* p, qreal pot) : parent_(p), position_(p ? p->center() : QPointF()), potency_(pot) {};
    virtual ~Power() = default;

    virtual void draw(QPainter& painter) const = 0;

    virtual void onSpawn(GameContext& ctx) {};
    virtual bool resolveBallCollision(Ball &other) = 0;
    virtual bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) = 0;

    qreal lifetime() const {return lifetime_;};
    virtual void decreaseLife(qreal dt) {lifetime_ -= dt;};

protected:
    qreal maxlife_;

    Ball* parent_ = nullptr;
    QPointF velocity_;
    QPointF position_;
    qreal lifetime_;
    qreal potency_ = 1.0;
};

#endif // POWER_H
