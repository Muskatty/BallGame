#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QPointF>
#include <QColor>
#include "powercolors.h"

struct WeaponConfig {
    qreal width = 16.0;
    qreal length = 60.0;
    qreal velocity = 270.0;
    int damage = 10;
};

struct BallConfig {
    qreal radius = 12.0;
    QPointF initialPosition;
    QPointF initialVelocity = QPointF(140, 140);
    int hp = 100;

    PowerType type;

    WeaponConfig weapon;
};

struct GameConfig {
    int windowWidth = 800;
    int windowHeight = 600;
    int cellSize = 20;

    int maxUpgradesCount = 6;

    qreal powersPotencyStart = 1.0;

    std::vector<BallConfig> ballConfigs;

    static GameConfig defaultConfig();
};

#endif // GAMECONFIG_H
