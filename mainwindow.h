#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BallGridWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    BallGridWidget* gameWidget = nullptr;
};

#endif // MAINWINDOW_H
