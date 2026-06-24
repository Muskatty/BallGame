#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>

class BallGridWidget;

#ifdef QT_DEBUG
class DebugWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DebugWidget(BallGridWidget* grid, QWidget *parent = nullptr);

private:
    BallGridWidget* grid;
};
#endif // QT_DEBUG

#endif // DEBUGWIDGET_H
