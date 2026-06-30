#ifndef POWERCOLORS_H
#define POWERCOLORS_H

#include <QColor>

enum class PowerType {
    Holy,
    Water,
    Thief,
    Earth,
    Count
};

static std::unordered_map<PowerType, QColor> powerToColor = {
    {PowerType::Holy, QColor(255, 251, 201)},
    {PowerType::Water, QColor(0, 191, 255)},
    {PowerType::Thief, QColor(158, 158, 158)},
    {PowerType::Earth, QColor(156, 74, 53)}
};

static std::unordered_map<PowerType, QString> powerToString = {
    {PowerType::Holy, QStringLiteral("Holy")},
    {PowerType::Water, QStringLiteral("Water")},
    {PowerType::Thief, QStringLiteral("Thief")},
    {PowerType::Earth, QStringLiteral("Earth")}
};

static std::unordered_map<QString, PowerType> stringToPower = {
    {QStringLiteral("Holy"), PowerType::Holy},
    {QStringLiteral("Water"), PowerType::Water},
    {QStringLiteral("Thief"), PowerType::Thief},
    {QStringLiteral("Earth"), PowerType::Earth}
};

#endif // POWERCOLORS_H
