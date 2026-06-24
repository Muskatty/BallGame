#include "ballgrid.h"

BallGridWidget::BallGridWidget(QWidget* parent) : QWidget(parent)
{
    setFixedSize(windowWidth, windowHeight);

    visited.resize(rows, std::vector<QColor>(cols, QColor(60, 60, 60)));
    for (int i = 0; i < rows / 2; i++) {
        for (int j = 0; j < cols / 2; j++) {
            visited[i][j] = Qt::red;
        }
    }
    for (int i = rows / 2 + 1; i < rows; i++) {
        for (int j = 0; j < cols / 2; j++) {
            visited[i][j] = Qt::green;
        }
    }

    for (int i = 0; i < rows / 2; i++) {
        for (int j = cols / 2 + 1; j < cols; j++) {
            visited[i][j] = Qt::yellow;
        }
    }

    for (int i = rows / 2 + 1; i < rows; i++) {
        for (int j = cols / 2 + 1; j < cols; j++) {
            visited[i][j] = Qt::blue;
        }
    }

    balls.push_back(Ball(QPointF(100.0, 100.0), QPointF(140.0, 140.0), 20.0));
    balls.push_back(Ball(QPointF(100.0, 500.0), QPointF(140.0, 140.0), 20.0));
    balls.push_back(Ball(QPointF(500.0, 100.0), QPointF(140.0, 140.0), 20.0));
    balls.push_back(Ball(QPointF(500.0, 500.0), QPointF(140.0, 140.0), 20.0));
    balls[1].setTraceColor(Qt::green);
    balls[2].setTraceColor(Qt::yellow);
    balls[3].setTraceColor(Qt::blue);
    field = QRectF(0.0, 0.0, windowWidth, windowHeight);

#ifdef QT_DEBUG
    debugWindow = new DebugWidget(this);
    debugWindow->show();
#endif

    timer.setInterval(16); // ~60 FPS
    connect(&timer, &QTimer::timeout, this, &BallGridWidget::onTick);
    timer.start();
}

void BallGridWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            QRect cellRect(x * cellSize, y * cellSize, cellSize - 1, cellSize - 1);

            painter.fillRect(cellRect, visited[y][x]);
        }
    }

    for (auto& power : powers) {
        power->draw(painter);
    }

    for (auto& upgrade : upgrades) {
        upgrade.draw(painter);
    }

    for (auto& ball : balls) {
        ball.draw(painter);
    }
}

void BallGridWidget::updateBalls(qreal dt) {
    for (int i = 0; i < balls.size();) {
        //check whether ball is dead and delete it
        if (balls[i].health() <= 0) {
            balls.erase(balls.begin() + i);
            continue;
        }

        //move and check if collides with field border
        balls[i].move(dt);
        balls[i].resolveFieldCollision(field);

        //check collision with weapons
        for (int j = 0; j < balls.size(); j++) {
            if (i == j) {
                continue;
            }
            balls[i].bounceOffWeapon(balls[j]);
            balls[i].getWeapon()->bounceOffWeapon(*balls[j].getWeapon());
        }

        //check collision with other balls
        for (int j = i + 1; j < balls.size(); j++) {
            balls[i].bounceOff(balls[j]);
        }

        //check collision with cells of other color
        int centerCol = static_cast<int>(balls[i].x()) / cellSize;
        int centerRow = static_cast<int>(balls[i].y()) / cellSize;

        int bCFS = balls[i].radius() / cellSize + 1;
        int upperBBound = std::max(0, centerRow - bCFS);
        int lowerBBound = std::min(rows - 1, centerRow + bCFS);
        int leftBBound = std::max(0, centerCol - bCFS);
        int rightBBound = std::min(cols - 1, centerCol + bCFS);

        bool collided = false;
        for (int x = leftBBound; x <= rightBBound && !collided; x++) {
            for (int y = upperBBound; y <= lowerBBound && !collided; y++) {
                QRectF cell(x * cellSize, y * cellSize, cellSize, cellSize);
                if (balls[i].traceColor() != visited[y][x]) {
                    if (balls[i].resolveCellCollision(cell)) {
                        collided = true;
                    }
                }
            }
        }

        i++;
    }
}

void BallGridWidget::updatePowers(qreal dt) {
    for (int i = 0; i < powers.size();) {
        if (powers[i]->lifetime() < 0.0) {
            powers.erase(powers.begin() + i);
            continue;
        }
        powers[i]->decreaseLife(dt);

        for (auto& ball : balls) {
            powers[i]->resolveBallCollision(ball);
        }

        powers[i]->resolveFieldCollision(visited, windowWidth, windowHeight);

        i++;
    }
}

void BallGridWidget::updateUpgrades() {
    for (auto& ball : balls) {
        for (int i = 0; i < upgrades.size();) {
            //check whether collision occured
            if (ball.resolveUpgradeCollision(upgrades[i])) {
                //if collided with power upgrade - spawn power
                if (upgrades[i].type() == UpgradeType::Power) {
                    powers.push_back(std::make_shared<HolyPower>(ball, powersPotency));
                }
                upgrades.erase(upgrades.begin() + i);
                continue;
            }
            i++;
        }
    }
}

void BallGridWidget::updateCells() {
    for (auto& ball : balls) {
        int centerCol = static_cast<int>(ball.x()) / cellSize;
        int centerRow = static_cast<int>(ball.y()) / cellSize;

        int wCFS = ball.weaponLen() / cellSize + 1;
        int upperWBound = std::max(0, centerRow - wCFS);
        int lowerWBound = std::min(rows - 1, centerRow + wCFS);
        int leftWBound = std::max(0, centerCol - wCFS);
        int rightWBound = std::min(cols - 1, centerCol + wCFS);

        for (int x = leftWBound; x <= rightWBound; x++) {
            for (int y = upperWBound; y <= lowerWBound; y++) {
                QRectF cell(x * cellSize, y * cellSize, cellSize, cellSize);
                if (ball.detectCellWeaponCollision(cell)) {
                    visited[y][x] = ball.traceColor();
                }
            }
        }
    }
}

void BallGridWidget::trySpawnUpgrade() {
    if (QRandomGenerator::global()->bounded(200) == 1 && upgrades.size() < 6) {
        upgrades.push_back(Upgrade({windowWidth, windowHeight}));
    }
}

void BallGridWidget::onTick()
{
    const double dt = 0.016;
    powersPotency += dt * 0.05;

    updateBalls(dt);
    updatePowers(dt);
    updateUpgrades();
    updateCells();

    trySpawnUpgrade();

    update();
}

#ifdef QT_DEBUG
void BallGridWidget::giveUpgrade(int idx, UpgradeType type) {
    if (idx >= balls.size()) {
        return;
    }
    balls[idx].applyUpgrade(type);
    if (type == UpgradeType::Power) {
        powers.push_back(std::make_shared<WaterPower>(balls[idx], powersPotency));
    }
}
#endif