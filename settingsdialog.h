#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "gameconfig.h"
#include <QSpinBox>
#include <QComboBox>

struct GameWidgets {
    QSpinBox* width = nullptr;
    QSpinBox* height = nullptr;
    QSpinBox* cell = nullptr;
    QSpinBox* upgrades = nullptr;
    QDoubleSpinBox* potency = nullptr;
};

struct CommonBallWidgets {
    QDoubleSpinBox* radius = nullptr;
    QDoubleSpinBox* velX = nullptr;
    QDoubleSpinBox* velY = nullptr;
    QSpinBox* hp = nullptr;
};

struct CommonWeaponWidgets {
    QDoubleSpinBox* width = nullptr;
    QDoubleSpinBox* length = nullptr;
    QDoubleSpinBox* vel = nullptr;
    QSpinBox* dmg = nullptr;
};

struct BallWidgets {
    QDoubleSpinBox* posX = nullptr;
    QDoubleSpinBox* posY = nullptr;
    QComboBox* type = nullptr;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(const GameConfig& cfg, QWidget* parent = nullptr);

    const GameConfig& config() const { return cfg;};

public slots:
    void saveConfig();

private:
    GameConfig cfg;

    GameWidgets gameWidgets;
    CommonBallWidgets commonBallWidgets;
    CommonWeaponWidgets commonWeaponWidgets;

    std::vector<BallWidgets> ballWidgets;
};

#endif // SETTINGSDIALOG_H
