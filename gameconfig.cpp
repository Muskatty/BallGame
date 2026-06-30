#include "gameconfig.h"

namespace {
WeaponConfig makeWeaponConfig(QPointF pos) {
    WeaponConfig cfg;
    cfg.width = 16.0;
    cfg.length = 60.0;
    cfg.velocity = 270.0;
    cfg.damage = 10;
    cfg.initialPosition = pos;
    return cfg;
}

BallConfig makeBallConfig(QPointF pos, PowerType type) {
    BallConfig cfg;
    cfg.radius = 20.0;
    cfg.initialPosition = pos;
    cfg.initialVelocity = {140.0, 140.0};
    cfg.hp = 100;
    cfg.type = type;
    cfg.weapon = makeWeaponConfig(pos);
    return cfg;
}
}

GameConfig GameConfig::defaultConfig() {
    GameConfig config;
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.cellSize = 20;
    config.ballConfigs = {
        makeBallConfig({100.0, 100.0}, PowerType::Holy),
        makeBallConfig({100.0, 500.0}, PowerType::Earth),
        makeBallConfig({500.0, 100.0}, PowerType::Thief),
        makeBallConfig({500.0, 500.0}, PowerType::Water)
    };
    return config;
}