#include "upgrade.h"
#include <QRandomGenerator>

Upgrade::Upgrade(QPointF fieldSize) {
    qreal x = QRandomGenerator::global()->bounded(fieldSize.x());
    qreal y = QRandomGenerator::global()->bounded(fieldSize.y());
    position = QPointF(x, y);

    int ty = QRandomGenerator::global()->bounded((int)UpgradeType::Count);
    t = static_cast<UpgradeType>(ty);
}

void Upgrade::draw(QPainter& painter)
{
    painter.setBrush(toColor[t]);
    painter.setPen(Qt::black);
    painter.drawEllipse(position, r, r);
}