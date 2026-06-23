#include "upgrade.h"
#include <QRandomGenerator>

Upgrade::Upgrade(QPointF fieldSize) {
    int lower = static_cast<int>(r);
    int upperX = static_cast<int>(fieldSize.x() - r);
    int upperY = static_cast<int>(fieldSize.y() - r);
    qreal x = QRandomGenerator::global()->bounded(lower, upperX);
    qreal y = QRandomGenerator::global()->bounded(lower, upperY);
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