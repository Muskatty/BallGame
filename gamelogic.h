#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "ball.h"
#include "power.h"
#include "gameconfig.h"

class GameLogic
{
public:
    GameLogic(const GameConfig& cfg);

    void applyConfig(const GameConfig& cfg);

    void draw(QPainter& painter) const;
    void update(qreal dt);

#ifdef QT_DEBUG
public:
    int ballCount() const { return balls.size();};

    void giveUpgrade(int idx, UpgradeType type);
#endif

private:
    void initField();
    void initBalls();

    void restart();

    void updateBalls(qreal dt);
    void updatePowers(qreal dt);
    void updateUpgrades();
    void updateCells();

    void trySpawnUpgrade();
    void createPower(Ball* ball);

private:
    GameConfig config;

    int cols;// = windowWidth / cellSize;
    int rows;// = windowHeight / cellSize;
    //static constexpr double ballRadius = 12.0;
    //static constexpr double weaponLen = 60.0;

    std::vector<std::unique_ptr<Ball>> balls;
    std::vector<Upgrade> upgrades;
    std::vector<std::unique_ptr<Power>> powers;
    QRectF field;
    std::vector<std::vector<QColor>> cells;

    qreal powersPotency = 1.0;
};

#endif // GAMELOGIC_H
