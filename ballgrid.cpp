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
        for (int j = 0; j < cols /2; j++) {
            visited[i][j] = Qt::green;
        }
    }

    for (int i = 0; i < rows / 2; i++) {
        for (int j = cols / 2 + 1; j< cols; j++) {
            visited[i][j] = Qt::yellow;
        }
    }

    balls.push_back(Ball(QPointF(100.0, 100.0), QPointF(140.0, 140.0), 20.0));
    balls.push_back(Ball(QPointF(100.0, 500.0), QPointF(140.0, 140.0), 20.0));
    balls.push_back(Ball(QPointF(500.0, 100.0), QPointF(140.0, 140.0), 20.0));
    balls[1].setTraceColor(Qt::green);
    balls[2].setTraceColor(Qt::yellow);
    field = QRectF(0.0, 0.0, windowWidth, windowHeight);

    timer.setInterval(16); // ~60 FPS
    connect(&timer, &QTimer::timeout, this, &BallGridWidget::onTick);
    timer.start();
}

void BallGridWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));

    // Сетка
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            QRect cellRect(x * cellSize, y * cellSize, cellSize - 1, cellSize - 1);

            painter.fillRect(cellRect, visited[y][x]);
        }
    }

    for (auto& upgrade : upgrades) {
        upgrade.draw(painter);
    }

    for (auto& ball : balls) {
        ball.draw(painter);
    }
}

void BallGridWidget::onTick()
{
    const double dt = 0.016;

    for (int i = 0; i < balls.size();) {
        if (balls[i].health() <= 0) {
            balls.remove(i);
            continue;
        }
        balls[i].move(dt);
        balls[i].resolveFieldCollision(field);

        for (int j = 0; j < balls.size(); j++) {
            balls[i].bounceOffWeapon(balls[j]);
            balls[i].getWeapon()->bounceOffWeapon(*balls[j].getWeapon());
        }

        for (int j = i + 1; j < balls.size(); j++) {
            balls[i].bounceOff(balls[j]);
        }

        for (int j = 0; j < upgrades.size();) {
            if (balls[i].resolveUpgradeCollision(upgrades[j])) {
                upgrades.removeAt(j);
                continue;
            }
            j++;
        }

        // Find cell that contains ball center
        int centerCol = static_cast<int>(balls[i].x()) / cellSize;
        int centerRow = static_cast<int>(balls[i].y()) / cellSize;

        // Find bounds of cell grid to check for collision with weapon
        int wCFS = balls[i].weaponLen() / cellSize + 1;
        int upperBound = std::max(0, centerRow - wCFS);
        int lowerBound = std::min(rows - 1, centerRow + wCFS);
        int leftBound = std::max(0, centerCol - wCFS);
        int rightBound = std::min(cols - 1, centerCol + wCFS);

        bool collided = false;
        //TODO: split cell coloring and cell collision
        for (int x = leftBound; x <= rightBound; x++) {
            for (int y = upperBound; y <= lowerBound; y++) {
                QRectF cell(x * cellSize, y * cellSize, cellSize, cellSize);
                if (balls[i].detectCellCollision(cell)) {
                    visited[y][x] = balls[i].traceColor();
                }
                if (balls[i].traceColor() != visited[y][x] && !collided) {
                    balls[i].resolveCellCollision(cell);
                }
            }
        }
        ++i;
    }

    if (QRandomGenerator::global()->bounded(200) == 1 && upgrades.size() < 6) {
        upgrades.push_back(Upgrade({windowWidth, windowHeight}));
    }

    update();
}