#ifndef THIEFPOWER_H
#define THIEFPOWER_H

#include "power.h"

static constexpr qreal thiefMaxLife = 6.33;

class ThiefPower : public Power
{
public:
    ThiefPower(Ball* p, qreal pot);

    void draw(QPainter& painter) const override;

    void onSpawn(GameContext& ctx) override;
    bool resolveBallCollision(Ball& other) override {return false;};
    bool resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) override {return false;};
    void decreaseLife(qreal dt) override;

private:
    void absorbUpgrades(std::vector<Upgrade>& upgrades);
    void applyUpgradeToParent(const Upgrade& up);
    void update(qreal dt);

private:
    std::vector<Upgrade> stolen;
};

#endif // THIEFPOWER_H
