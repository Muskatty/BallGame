#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>

namespace
{
void fillTypeCombo(QComboBox* box) {
    for (auto& [type, str] : powerToString) {
        box->addItem(str);
    }
}

void setup(QSpinBox* box, int min, int max) {
    box->setRange(min, max);
}

void setup(QDoubleSpinBox* box,
            qreal min,
            qreal max,
            qreal step = 1.0,
            int decimals = 1) {
    box->setRange(min, max);
    box->setSingleStep(step);
    box->setDecimals(decimals);
}
}

SettingsDialog::SettingsDialog(const GameConfig& cfg, QWidget* parent) : cfg(cfg) {
    auto* layout = new QVBoxLayout(this);

    auto* gameSettings = new QGroupBox(QStringLiteral("Game settings"), this);
    {
        auto* h = new QHBoxLayout(gameSettings);

        gameWidgets.width = new QSpinBox();
        gameWidgets.height = new QSpinBox();
        gameWidgets.cell = new QSpinBox();
        gameWidgets.upgrades = new QSpinBox();
        gameWidgets.potency = new QDoubleSpinBox();

        setup(gameWidgets.width, 800, 2000);
        setup(gameWidgets.height, 600, 2000);
        setup(gameWidgets.cell, 1, 100);
        setup(gameWidgets.upgrades, 0, 100);
        setup(gameWidgets.potency, 0.0, 100.0, 0.1);

        gameWidgets.width->setValue(cfg.windowWidth);
        gameWidgets.height->setValue(cfg.windowHeight);
        gameWidgets.cell->setValue(cfg.cellSize);
        gameWidgets.upgrades->setValue(cfg.maxUpgradesCount);
        gameWidgets.potency->setValue(cfg.powersPotencyStart);

        h->addWidget(gameWidgets.width);
        h->addWidget(gameWidgets.height);
        h->addWidget(gameWidgets.cell);
        h->addWidget(gameWidgets.upgrades);
        h->addWidget(gameWidgets.potency);

        layout->addWidget(gameSettings);
    }

    auto* commonBall = new QGroupBox(QStringLiteral("Common ball settings"), this);
    {
        auto* h = new QHBoxLayout(commonBall);

        commonBallWidgets.radius = new QDoubleSpinBox();
        commonBallWidgets.velX = new QDoubleSpinBox();
        commonBallWidgets.velY = new QDoubleSpinBox();
        commonBallWidgets.hp = new QSpinBox();

        setup(commonBallWidgets.radius, 16.0, 100.0);
        setup(commonBallWidgets.velX, -cfg.windowWidth, cfg.windowWidth);
        setup(commonBallWidgets.velY, -cfg.windowHeight, cfg.windowHeight);
        setup(commonBallWidgets.hp, 0, 1000);

        commonBallWidgets.radius->setValue(cfg.ballConfigs[0].radius);
        commonBallWidgets.velX->setValue(cfg.ballConfigs[0].initialVelocity.x());
        commonBallWidgets.velY->setValue(cfg.ballConfigs[0].initialVelocity.y());
        commonBallWidgets.hp->setValue(cfg.ballConfigs[0].hp);

        h->addWidget(commonBallWidgets.radius);
        h->addWidget(commonBallWidgets.velX);
        h->addWidget(commonBallWidgets.velY);
        h->addWidget(commonBallWidgets.hp);

        layout->addWidget(commonBall);
    }

    auto* commonWeapon = new QGroupBox(QStringLiteral("Common weapon settings"), this);
    {
        auto* h = new QHBoxLayout(commonWeapon);

        commonWeaponWidgets.width = new QDoubleSpinBox();
        commonWeaponWidgets.length = new QDoubleSpinBox();
        commonWeaponWidgets.vel = new QDoubleSpinBox();
        commonWeaponWidgets.dmg = new QSpinBox();

        setup(commonWeaponWidgets.width, 1.0, 200.0);
        setup(commonWeaponWidgets.length, 1.0, 1000.0);
        setup(commonWeaponWidgets.vel, 1.0, 720.0);
        setup(commonWeaponWidgets.dmg, 1, 1000);

        commonWeaponWidgets.width->setValue(cfg.ballConfigs[0].weapon.width);
        commonWeaponWidgets.length->setValue(cfg.ballConfigs[0].weapon.length);
        commonWeaponWidgets.vel->setValue(cfg.ballConfigs[0].weapon.velocity);
        commonWeaponWidgets.dmg->setValue(cfg.ballConfigs[0].weapon.damage);

        h->addWidget(commonWeaponWidgets.width);
        h->addWidget(commonWeaponWidgets.length);
        h->addWidget(commonWeaponWidgets.vel);
        h->addWidget(commonWeaponWidgets.dmg);

        layout->addWidget(commonWeapon);
    }

    for (int i = 0; i < cfg.ballConfigs.size(); i++) {
        const auto& ballCfg = cfg.ballConfigs[i];

        auto* box = new QGroupBox(QString("Ball %1").arg(i + 1), this);
        auto* h = new QHBoxLayout(box);

        ballWidgets.emplace_back();
        auto& widgets = ballWidgets.back();

        widgets.posX = new QDoubleSpinBox();
        widgets.posY = new QDoubleSpinBox();
        widgets.type = new QComboBox();

        setup(widgets.posX, 0.0, cfg.windowWidth * 2.0);
        setup(widgets.posY, 0.0, cfg.windowHeight * 2.0);
        fillTypeCombo(widgets.type);

        widgets.posX->setValue(cfg.ballConfigs[i].initialPosition.x());
        widgets.posY->setValue(cfg.ballConfigs[i].initialPosition.y());
        widgets.type->setCurrentText(powerToString.at(cfg.ballConfigs[i].type));

        h->addWidget(widgets.posX);
        h->addWidget(widgets.posY);
        h->addWidget(widgets.type);

        //box->setLayout(h);
        layout->addWidget(box);
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    adjustSize();

    connect(this, &QDialog::accepted, this, &SettingsDialog::saveConfig);
}

void SettingsDialog::saveConfig() {
    cfg.windowWidth = gameWidgets.width->value();
    cfg.windowHeight = gameWidgets.height->value();
    cfg.cellSize = gameWidgets.cell->value();
    cfg.maxUpgradesCount = gameWidgets.upgrades->value();
    cfg.powersPotencyStart = gameWidgets.potency->value();

    for (int i = 0; i < ballWidgets.size(); i++) {
        auto& cfgBall = cfg.ballConfigs[i];
        auto& widgets = ballWidgets[i];

        cfgBall.radius = commonBallWidgets.radius->value();
        cfgBall.initialPosition = {
            widgets.posX->value(),
            widgets.posY->value()
        };
        cfgBall.initialVelocity = {
            commonBallWidgets.velX->value(),
            commonBallWidgets.velY->value()
        };
        cfgBall.hp = commonBallWidgets.hp->value();
        cfgBall.type = stringToPower.at(widgets.type->currentText());

        auto& weapon = cfgBall.weapon;

        weapon.width = commonWeaponWidgets.width->value();
        weapon.length = commonWeaponWidgets.length->value();
        weapon.damage = commonWeaponWidgets.dmg->value();
        weapon.velocity = commonWeaponWidgets.vel->value();
    }
}
