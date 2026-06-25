#include "holypower.h"

void HolyPower::setTouching(const Ball* b, bool value) {
    if (value) {
        touchedBalls.insert(b);
    } else {
        touchedBalls.remove(b);
    }
}

void HolyPower::draw(QPainter& painter) {
    painter.setBrush(powerToColor[PowerType::Holy]);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(position, radius(), radius());
}

bool HolyPower::resolveBallCollision(Ball& other) {
    if (&parent == &other) {
        return false;
    }

    Collision::CollisionInfo coll = Collision::circleCircle(position, radius(), other.center(), other.radius());

    if (!coll.hit) {
        setTouching(&other, false);
        return false;
    }

    bool firstHit = !isTouching(&other);

    if (firstHit) {
        other.takeDamage(parent.weaponDmg());
        setTouching(&other, true);
    }

    return true;
}

bool HolyPower::resolveFieldCollision(std::vector<std::vector<QColor>>& field, qreal width, qreal height) {
    int rows = field.size();
    int cols = field[0].size();
    qreal cellSize = width / cols;

    int centerCol = static_cast<int>(position.x()) / cellSize;
    int centerRow = static_cast<int>(position.y()) / cellSize;

    int pCFS = radius() / cellSize + 1;
    int upperBound = std::max(0, centerRow - pCFS);
    int lowerBound = std::min(rows - 1, centerRow + pCFS);
    int leftBound = std::max(0, centerCol - pCFS);
    int rightBound = std::min(cols - 1, centerCol + pCFS);

    QTransform transform;

    for (int x = leftBound; x <= rightBound; x++) {
        for (int y = upperBound; y <= lowerBound; y++) {
            QRectF cell(x * cellSize, y * cellSize, cellSize, cellSize);
            Collision::CollisionInfo coll = Collision::circleRect(position, radius(), cell, transform);
            if (coll.hit) {
                field[y][x] = parent.traceColor();
            }
        }
    }

    return true;
}