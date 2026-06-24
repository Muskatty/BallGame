#include "debugwidget.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

#include "ballgrid.h"

#ifdef QT_DEBUG

DebugWidget::DebugWidget(BallGridWidget *grid_, QWidget *parent)
    : QWidget{parent}, grid(grid_)
{
    auto* layout = new QGridLayout(this);

    for (int i = 0; i < 4; i++) {
        auto* box = new QGroupBox(QString("Ball %1").arg(i + 1), this);
        auto* v = new QVBoxLayout(box);

        auto* healthBtn = new QPushButton("Apply Health");
        auto* strengthBtn = new QPushButton("Apply Strength");
        auto* speedBtn = new QPushButton("Apply Speed");
        auto* spinBtn = new QPushButton("Apply Spin");
        auto* sizeBtn = new QPushButton("Apply Size");
        auto* powerBtn = new QPushButton("Apply Power");

        connect(healthBtn, &QPushButton::clicked, this, [this, i] {
            grid->giveUpgrade(i, UpgradeType::Health);
        });
        connect(strengthBtn, &QPushButton::clicked, this, [this, i] {
            grid->giveUpgrade(i, UpgradeType::Strength);
        });
        connect(speedBtn, &QPushButton::clicked, this, [this, i] {
            grid->giveUpgrade(i, UpgradeType::Speed);
        });
        connect(spinBtn, &QPushButton::clicked, this, [this, i] {
            grid->giveUpgrade(i, UpgradeType::SpinSpeed);
        });
        connect(sizeBtn, &QPushButton::clicked, this, [this, i] {
            grid->giveUpgrade(i, UpgradeType::WeaponSize);
        });
        connect(powerBtn, &QPushButton::clicked, this, [this, i] {
            grid->giveUpgrade(i, UpgradeType::Power);
        });

        v->addWidget(healthBtn);
        v->addWidget(strengthBtn);
        v->addWidget(speedBtn);
        v->addWidget(spinBtn);
        v->addWidget(sizeBtn);
        v->addWidget(powerBtn);

        box->setLayout(v);
        layout->addWidget(box, i / 2, i % 2);
    }

    setLayout(layout);
}

#endif