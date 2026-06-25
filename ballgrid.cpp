#include "ballgrid.h"

BallGridWidget::BallGridWidget(QWidget* parent) : QWidget(parent)
{
    setFixedSize(GameLogic::windowWidth, GameLogic::windowHeight);

#ifdef QT_DEBUG
    debugWindow = new DebugWidget(&engine, parent);
    debugWindow->show();
#endif

    timer.setInterval(16); // ~60 FPS
    connect(&timer, &QTimer::timeout, this, &BallGridWidget::onTick);
    timer.start();
}

void BallGridWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    engine.draw(painter);
}

void BallGridWidget::onTick()
{
    const double dt = 0.016;
    engine.update(dt);

    update();
}