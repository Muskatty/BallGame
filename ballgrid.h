#ifndef BALLGRID_H
#define BALLGRID_H

#include "ball.h"
#include "power.h"
#include "holypower.h"
#include "waterpower.h"
#include "debugwidget.h"

#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <QPainter>
#include <vector>
#include <QRandomGenerator>

//TODO:: SPLIT GAME LOGIC AND WIDGET
class BallGridWidget : public QWidget {
public:
    BallGridWidget(QWidget *parent = nullptr);
#ifdef QT_DEBUG
public:
    int ballCount() const { return balls.size();};

    void giveUpgrade(int idx, UpgradeType type);
private:
    DebugWidget* debugWindow = nullptr;
#endif
protected:
    void paintEvent(QPaintEvent*) override;
private:
    void updateBalls(qreal dt);
    void updatePowers(qreal dt);
    void updateUpgrades();
    void updateCells();

    void trySpawnUpgrade();

    void onTick();

private:
    static constexpr int windowWidth = 800;
    static constexpr int windowHeight = 600;
    static constexpr int cellSize = 20;
    static constexpr int cols = windowWidth / cellSize;
    static constexpr int rows = windowHeight / cellSize;
    static constexpr double ballRadius = 12.0;
    static constexpr double weaponLen = 60.0;

    qreal powersPotency = 1.0;

    QTimer timer;
    QVector<Ball> balls;
    QVector<Upgrade> upgrades;
    QVector<std::shared_ptr<Power>> powers;
    QRectF field;
    std::vector<std::vector<QColor>> visited;
};

#endif
