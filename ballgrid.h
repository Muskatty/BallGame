#ifndef BALLGRID_H
#define BALLGRID_H

#include "ball.h"

#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <QPainter>
#include <vector>

class BallGridWidget : public QWidget {
public:
    BallGridWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    void onTick();

private:
    static constexpr int windowWidth = 800;
    static constexpr int windowHeight = 600;
    static constexpr int cellSize = 20;
    static constexpr int cols = windowWidth / cellSize;
    static constexpr int rows = windowHeight / cellSize;
    static constexpr double ballRadius = 12.0;
    static constexpr double weaponLen = 60.0;

    QTimer timer;
    QVector<Ball> balls;
    QRectF field;
    std::vector<std::vector<QColor>> visited;
};

#endif
