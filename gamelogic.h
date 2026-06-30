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

    const GameConfig& config() const {return config_;};

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
    GameConfig config_;

    int cols;
    int rows;

    std::vector<std::unique_ptr<Ball>> balls;
    std::vector<Upgrade> upgrades;
    std::vector<std::unique_ptr<Power>> powers;
    QRectF field;
    std::vector<std::vector<QColor>> cells;

    qreal powersPotency = 1.0;
};

#endif // GAMELOGIC_H
