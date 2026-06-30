#ifndef BALLGRID_H
#define BALLGRID_H

#include "gamelogic.h"
#include "debugwidget.h"

#include <QWidget>
#include <QTimer>
#include <QPointF>
#include <QPainter>
#include <QRandomGenerator>

class BallGridWidget : public QWidget {
public:
    BallGridWidget(QWidget *parent = nullptr);

#ifdef QT_DEBUG
private:
    DebugWidget* debugWindow = nullptr;
#endif

protected:
    void paintEvent(QPaintEvent*) override;

private:
    void onTick();

private:
    std::unique_ptr<GameLogic> engine;

    QTimer timer;
};

#endif
