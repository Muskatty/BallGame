#include "gamelogic.h"
#include "powerfactory.h"
#include <QRandomGenerator>

GameLogic::GameLogic(const GameConfig& cfg) {
    config_ = cfg;
    rows = config_.windowHeight / config_.cellSize;
    cols = config_.windowWidth / config_.cellSize;
    initBalls();
    initField();
}

void GameLogic::applyConfig(const GameConfig& cfg) {
    config_ = cfg;
    restart();
}

void GameLogic::initField() {
    cells.resize(rows, std::vector<QColor>(cols, QColor(60, 60, 60)));
    field = QRectF(0.0, 0.0, config_.windowWidth, config_.windowHeight);

    for (int i = 0; i < rows / 2; i++) {
        for (int j = 0; j < cols / 2; j++) {
            cells[i][j] = balls[0]->traceColor();
        }
    }
    for (int i = rows / 2 + 1; i < rows; i++) {
        for (int j = 0; j < cols / 2; j++) {
            cells[i][j] = balls[1]->traceColor();
        }
    }

    for (int i = 0; i < rows / 2; i++) {
        for (int j = cols / 2 + 1; j < cols; j++) {
            cells[i][j] = balls[2]->traceColor();
        }
    }

    for (int i = rows / 2 + 1; i < rows; i++) {
        for (int j = cols / 2 + 1; j < cols; j++) {
            cells[i][j] = balls[3]->traceColor();
        }
    }
}

//TODO: Make normal ball and maybe field init
void GameLogic::initBalls() {
    for (const auto& ballConf : config_.ballConfigs) {
        balls.push_back(std::make_unique<Ball>(ballConf));
    }
    // balls.push_back(std::make_unique<Ball>(QPointF(100.0, 100.0), QPointF(140.0, 140.0), 20.0, PowerType::Holy));
    // balls.push_back(std::make_unique<Ball>(QPointF(100.0, 500.0), QPointF(140.0, 140.0), 20.0, PowerType::Water));
    // balls.push_back(std::make_unique<Ball>(QPointF(500.0, 100.0), QPointF(140.0, 140.0), 20.0, PowerType::Thief));
    // balls.push_back(std::make_unique<Ball>(QPointF(500.0, 500.0), QPointF(140.0, 140.0), 20.0, PowerType::Earth));
    //balls[1]->setTraceColor(Qt::green);
    //balls[2]->setTraceColor(Qt::yellow);
    //balls[3]->setTraceColor(Qt::blue);
}

void GameLogic::restart() {
    balls.clear();
    powers.clear();
    upgrades.clear();

    rows = config_.windowHeight / config_.cellSize;
    cols = config_.windowWidth / config_.cellSize;
    initBalls();
    initField();
}

void GameLogic::draw(QPainter& painter) const {
    painter.fillRect(field, QColor(30, 30, 30));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            QRect cellRect(x * config_.cellSize, y * config_.cellSize, config_.cellSize - 1, config_.cellSize - 1);

            painter.fillRect(cellRect, cells[y][x]);
        }
    }

    for (auto& power : powers) {
        power->draw(painter);
    }

    for (auto& upgrade : upgrades) {
        upgrade.draw(painter);
    }

    for (auto& ball : balls) {
        ball->draw(painter);
    }
}

void GameLogic::update(qreal dt) {
    powersPotency += dt * 0.05;

    updateBalls(dt);
    updatePowers(dt);
    updateUpgrades();
    updateCells();

    trySpawnUpgrade();
}

void GameLogic::updateBalls(qreal dt) {
    for (int i = 0; i < balls.size();) {
        //check whether ball is dead and delete it
        if (balls[i]->health() <= 0) {
            balls.erase(balls.begin() + i);
            continue;
        }

        //move and check if collides with field border
        balls[i]->move(dt);
        balls[i]->resolveFieldCollision(field);

        //check collision with weapons
        for (int j = 0; j < balls.size(); j++) {
            if (i == j) {
                continue;
            }
            balls[i]->bounceOffWeapon(*balls[j]);
            balls[i]->getWeapon()->bounceOffWeapon(*balls[j]->getWeapon());
        }

        //check collision with other balls
        for (int j = i + 1; j < balls.size(); j++) {
            balls[i]->bounceOffBall(*balls[j]);
        }

        //check collision with cells of other color
        int centerCol = static_cast<int>(balls[i]->x()) / config_.cellSize;
        int centerRow = static_cast<int>(balls[i]->y()) / config_.cellSize;

        int bCFS = balls[i]->radius() / config_.cellSize + 1;
        int upperBBound = std::max(0, centerRow - bCFS);
        int lowerBBound = std::min(rows - 1, centerRow + bCFS);
        int leftBBound = std::max(0, centerCol - bCFS);
        int rightBBound = std::min(cols - 1, centerCol + bCFS);

        bool collided = false;
        for (int x = leftBBound; x <= rightBBound && !collided; x++) {
            for (int y = upperBBound; y <= lowerBBound && !collided; y++) {
                QRectF cell(x * config_.cellSize, y * config_.cellSize, config_.cellSize, config_.cellSize);
                if (balls[i]->traceColor() != cells[y][x]) {
                    if (balls[i]->resolveCellCollision(cell)) {
                        collided = true;
                    }
                }
            }
        }

        i++;
    }
}

void GameLogic::updatePowers(qreal dt) {
    for (int i = 0; i < powers.size();) {
        if (powers[i]->lifetime() < 0.0) {
            powers.erase(powers.begin() + i);
            continue;
        }
        powers[i]->decreaseLife(dt);

        for (auto& ball : balls) {
            powers[i]->resolveBallCollision(*ball);
        }

        powers[i]->resolveFieldCollision(cells, config_.windowWidth, config_.windowHeight);

        i++;
    }
}

void GameLogic::createPower(Ball* ball) {
    if (!ball) {
        return;
    }

    auto power = PowerFactory::createPower(ball->powerType(), ball, powersPotency);
    if (power) {
        GameContext ctx{balls, upgrades, powers, cells, powersPotency};
        power->onSpawn(ctx);
        powers.push_back(std::move(power));
    }
}

void GameLogic::updateUpgrades() {
    for (auto& ball : balls) {
        for (int i = 0; i < upgrades.size();) {
            if (ball->resolveUpgradeCollision(upgrades[i])) {
                const UpgradeType type = upgrades[i].type();
                upgrades.erase(upgrades.begin() + i);

                //if collided with power upgrade - spawn power
                if (type == UpgradeType::Power) {
                    createPower(ball.get());
                }

                continue;
            }
            i++;
        }
    }
}

void GameLogic::updateCells() {
    for (auto& ball : balls) {
        int centerCol = static_cast<int>(ball->x()) / config_.cellSize;
        int centerRow = static_cast<int>(ball->y()) / config_.cellSize;

        int wCFS = ball->weaponLen() / config_.cellSize + 1;
        int upperWBound = std::max(0, centerRow - wCFS);
        int lowerWBound = std::min(rows - 1, centerRow + wCFS);
        int leftWBound = std::max(0, centerCol - wCFS);
        int rightWBound = std::min(cols - 1, centerCol + wCFS);

        for (int x = leftWBound; x <= rightWBound; x++) {
            for (int y = upperWBound; y <= lowerWBound; y++) {
                QRectF cell(x * config_.cellSize, y * config_.cellSize, config_.cellSize, config_.cellSize);
                if (ball->detectCellWeaponCollision(cell)) {
                    cells[y][x] = ball->traceColor();
                }
            }
        }
    }
}

void GameLogic::trySpawnUpgrade() {
    if (QRandomGenerator::global()->bounded(200) == 1 && upgrades.size() < config_.maxUpgradesCount) {
        upgrades.push_back(Upgrade({static_cast<qreal>(config_.windowWidth), static_cast<qreal>(config_.windowHeight)}));
    }
}

#ifdef QT_DEBUG
void GameLogic::giveUpgrade(int idx, UpgradeType type) {
    if (idx >= balls.size() || idx >= balls.size()) {
        return;
    }
    balls[idx]->applyUpgrade(type);
    if (type == UpgradeType::Power) {
        createPower(balls[idx].get());
    }
}
#endif
