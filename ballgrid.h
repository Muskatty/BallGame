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
public:
    int ballCount() const { return engine.ballCount();};

    void giveUpgrade(int idx, UpgradeType type) {engine.giveUpgrade(idx, type);};
private:
    DebugWidget* debugWindow = nullptr;
#endif

protected:
    void paintEvent(QPaintEvent*) override;

private:
    void onTick();

private:
    GameLogic engine;

    QTimer timer;
};

#endif
