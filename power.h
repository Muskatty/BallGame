#ifndef POWER_H
#define POWER_H

#include <QPolygonF>
#include <QPainter>
#include "ball.h"
#include "gamecontext.h"

enum class PowerType {
    Holy,
    Water,
    Thief,
    Count
};

static std::unordered_map<PowerType, QColor> powerToColor = {
    {PowerType::Holy, QColor(255, 251, 201)},
    {PowerType::Water, QColor(0, 191, 255)},
    {PowerType::Thief, QColor(158, 158, 158)}
};

class Power
{
public:
    Power(Ball& p, qreal pot) : parent(p), position(p.center()), potency(pot) {};
    virtual ~Power() {};

    virtual void draw(QPainter& painter) const = 0;

    virtual void onSpawn(GameContext& ctx) {};
    virtual bool resolveBallCollision(Ball &other) = 0;
    virtual bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) = 0;

    qreal lifetime() const {return lifet;};
    virtual void decreaseLife(qreal dt) {lifet -= dt;};

protected:
    qreal maxlife;

    Ball& parent;
    qreal vel;
    QPointF position;
    qreal lifet;
    qreal potency = 1.0;
};

#endif // POWER_H
