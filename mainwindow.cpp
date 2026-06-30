#include "mainwindow.h"
#include "ballgrid.h"

#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    gameWidget = new BallGridWidget(this);
    setCentralWidget(gameWidget);

    auto* gameMenu = menuBar()->addMenu(QStringLiteral("Game"));

    auto* settingsAction = gameMenu->addAction(QStringLiteral("Settings"));
    connect(settingsAction, &QAction::triggered, this, [this] {
        gameWidget->openSettings();
    });
}
