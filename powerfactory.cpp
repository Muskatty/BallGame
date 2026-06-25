#include "powerfactory.h"
#include "holypower.h"
#include "waterpower.h"

std::unique_ptr<Power> PowerFactory::createPower(PowerType type, Ball& b, qreal potency) {
    switch (type) {
    case PowerType::Holy:
        return std::make_unique<HolyPower>(b, potency);
    case PowerType::Water:
        return std::make_unique<WaterPower>(b, potency);
    }

    return nullptr;
}
