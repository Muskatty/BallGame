#include "upgrade.h"
#include <QRandomGenerator>

Upgrade::Upgrade(QPointF fieldSize) {
    int lower = static_cast<int>(radius_);
    int upperX = static_cast<int>(fieldSize.x() - radius_);
    int upperY = static_cast<int>(fieldSize.y() - radius_);
    qreal x = QRandomGenerator::global()->bounded(lower, upperX);
    qreal y = QRandomGenerator::global()->bounded(lower, upperY);
    position_ = QPointF(x, y);

    int ty = QRandomGenerator::global()->bounded((int)UpgradeType::Count);
    type_ = static_cast<UpgradeType>(ty);
}

void Upgrade::draw(QPainter& painter) const
{
    painter.setBrush(toColor[type_]);
    painter.setPen(Qt::black);
    painter.drawEllipse(position_, radius_, radius_);
}