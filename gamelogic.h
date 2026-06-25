#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "ball.h"
#include "power.h"

class GameLogic
{
public:
    GameLogic();

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

    void updateBalls(qreal dt);
    void updatePowers(qreal dt);
    void updateUpgrades();
    void updateCells();

    void trySpawnUpgrade();
    void createPower(Ball& ball);

public:
    static constexpr int windowWidth = 800;
    static constexpr int windowHeight = 600;

private:
    static constexpr int cellSize = 20;
    static constexpr int cols = windowWidth / cellSize;
    static constexpr int rows = windowHeight / cellSize;
    static constexpr double ballRadius = 12.0;
    static constexpr double weaponLen = 60.0;

    std::vector<Ball> balls;
    std::vector<Upgrade> upgrades;
    std::vector<std::unique_ptr<Power>> powers;
    QRectF field;
    std::vector<std::vector<QColor>> cells;

    qreal powersPotency = 1.0;
};

#endif // GAMELOGIC_H
