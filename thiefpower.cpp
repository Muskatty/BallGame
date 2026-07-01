#include "thiefpower.h"
#include "geometry.h"

ThiefPower::ThiefPower(Ball* p, qreal pot) : Power(p, pot) {
    maxlife_ = thiefMaxLife;
    lifetime_ = maxlife_;
}

void ThiefPower::draw(QPainter& painter) const {
    QPointF center = parent_->center();

    painter.save();
    painter.setPen(powerToColor[PowerType::Thief]);

    for (const Upgrade& up : stolen) {
        painter.drawLine(center, up.pos());
        up.draw(painter);
    }

    painter.restore();
}

void ThiefPower::onSpawn(GameContext& ctx) {
    absorbUpgrades(ctx.upgrades);
}

void ThiefPower::decreaseLife(qreal dt) {
    lifetime_ -= dt;
    update(dt);
}

void ThiefPower::absorbUpgrades(std::vector<Upgrade>& upgrades) {
    for (Upgrade& up : upgrades) {
        stolen.push_back(up);
    }
    upgrades.clear();
}

void ThiefPower::applyUpgradeToParent(const Upgrade& up) {
    parent_->applyUpgrade(up.type());
}

void ThiefPower::update(qreal dt) {
    const QPointF target = parent_->center();
    const qreal pullSpeed = 650.0;
    const qreal snapDist = parent_->radius();

    for (int i = 0; i < stolen.size();) {
        Upgrade &up = stolen[i];

        QPointF diff = target - up.pos();
        qreal dist = Geometry::length(diff);

        if (dist <= snapDist) {
            applyUpgradeToParent(up);
            stolen.erase(stolen.begin() + i);
            continue;

        }

        QPointF dir = Geometry::normalized(diff);
        qreal step = pullSpeed * dt;

        if (step >= dist) {
            up.setPos(target);
        } else {
            up.setPos(up.pos() + dir * step);
        }

        i++;
    }

    if (stolen.size() == 0) {
        lifetime_ = -1.0;
    }
}
