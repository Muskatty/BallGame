#include "ballgrid.h"
#include "settingsdialog.h"

BallGridWidget::BallGridWidget(QWidget* parent) : QWidget(parent)
{
    const GameConfig config = GameConfig::defaultConfig();

    engine = std::make_unique<GameLogic>(config);

    setFixedSize(config.windowWidth, config.windowHeight);

#ifdef QT_DEBUG
    debugWindow = new DebugWidget(engine.get(), parent);
    debugWindow->show();
#endif

    timer.setInterval(16); // ~60 FPS
    connect(&timer, &QTimer::timeout, this, &BallGridWidget::onTick);
    timer.start();
}

void BallGridWidget::openSettings()
{
    SettingsDialog dlg(engine->config(), this);
    if (dlg.exec() == QDialog::Accepted) {
        engine->applyConfig(dlg.config());
        setFixedSize(engine->config().windowWidth, engine->config().windowHeight);
    }
}

void BallGridWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    engine->draw(painter);
}

void BallGridWidget::onTick()
{
    const double dt = 0.016;
    engine->update(dt);

    update();
}