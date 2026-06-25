#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>

class GameLogic;

#ifdef QT_DEBUG
class DebugWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DebugWidget(GameLogic* grid, QWidget *parent = nullptr);

private:
    GameLogic* grid;
};
#endif // QT_DEBUG

#endif // DEBUGWIDGET_H
