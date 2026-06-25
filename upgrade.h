#ifndef UPGRADE_H
#define UPGRADE_H

#include <QPointF>
#include <QColor>
#include <QPainter>

enum class UpgradeType {
    Strength,
    Speed,
    SpinSpeed,
    Health,
    WeaponSize,
    Power,
    Count
};

static std::unordered_map<UpgradeType, QColor> toColor =
    {{UpgradeType::Strength, QColor(220, 20, 60)},
     {UpgradeType::Speed, QColor(0, 255, 255)},
     {UpgradeType::SpinSpeed, QColor(255, 215, 0)},
     {UpgradeType::Health, QColor(154, 205, 50)},
     {UpgradeType::WeaponSize, QColor(128, 0, 128)},
     {UpgradeType::Power, QColor(255, 26, 255)}};

constexpr int HealthBonus = 50;
constexpr int StrengthBonus = 5;
constexpr qreal SpeedPercentBonus = 0.10;
constexpr qreal SpinSpeedBonus = 45.0;
constexpr qreal WeaponSizeBonus = 16.0;


class Upgrade
{
public:
    Upgrade(QPointF fieldSize);

    QPointF pos() const {return position;};
    qreal radius() const {return r;};
    UpgradeType type() const {return t;};

    void setPos(QPointF newPos) {position = newPos;};

    void draw(QPainter& painter) const;

private:
    QPointF position;
    qreal r = 12;
    UpgradeType t;
};

#endif // UPGRADE_H
