#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <QColor>
#include <memory>

class Ball;
class Upgrade;
class Power;

struct GameContext {
    std::vector<std::unique_ptr<Ball>>& balls;
    std::vector<Upgrade>& upgrades;
    std::vector<std::unique_ptr<Power>>& powers;
    std::vector<std::vector<QColor>>& cells;
    qreal powersPotency;
};

#endif // GAMECONTEXT_H
