#ifndef POWER_H
#define POWER_H

#include <QPolygonF>
#include <QPainter>
#include "ball.h"
#include "geometry.h"

enum class PowerType {
    Holy,
    Count
};

static std::unordered_map<PowerType, QColor> powerToColor = {
    {PowerType::Holy, QColor(255, 251, 201)}
};

class Power
{
public:
    Power(Ball& p) : parent(p), position(p.center()) {};
    Power(Ball& p, qreal pot) : parent(p), position(p.center()), potency(pot) {};
    virtual ~Power() {};

    virtual void draw(QPainter& painter) = 0;
    virtual bool resolveBallCollision(Ball &other) = 0;
    virtual bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) = 0;

    qreal lifetime() {return lifet;};
    void decreaseLife(qreal dt) {lifet -= dt;};

protected:
    static constexpr qreal maxlife = 1.0;

    Ball& parent;
    qreal vel;
    QPointF position;
    qreal lifet = maxlife;
    qreal potency = 1.0;
};

#endif // POWER_H
