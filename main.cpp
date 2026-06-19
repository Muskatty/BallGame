#include "ballgrid.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    BallGridWidget w;
    w.setWindowTitle(QStringLiteral("Ball Grid"));
    w.show();

    return app.exec();
}
