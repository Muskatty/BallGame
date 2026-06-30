#ifndef POWER_H
#define POWER_H

#include <QPolygonF>
#include <QPainter>
#include "ball.h"
#include "gamecontext.h"

class Power
{
public:
    Power(Ball* p, qreal pot) : parent(p), position(p ? p->center() : QPointF()), potency(pot) {};
    virtual ~Power() = default;

    virtual void draw(QPainter& painter) const = 0;

    virtual void onSpawn(GameContext& ctx) {};
    virtual bool resolveBallCollision(Ball &other) = 0;
    virtual bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) = 0;

    qreal lifetime() const {return lifet;};
    virtual void decreaseLife(qreal dt) {lifet -= dt;};

protected:
    qreal maxlife;

    Ball* parent = nullptr;
    QPointF vel;
    QPointF position;
    qreal lifet;
    qreal potency = 1.0;
};

#endif // POWER_H
