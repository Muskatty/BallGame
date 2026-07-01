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

    QPointF pos() const {return position_;};
    qreal radius() const {return radius_;};
    UpgradeType type() const {return type_;};

    void setPos(QPointF newPos) {position_ = newPos;};

    void draw(QPainter& painter) const;

private:
    QPointF position_;
    qreal radius_ = 12;
    UpgradeType type_;
};

#endif // UPGRADE_H
