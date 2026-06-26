#ifndef POWERFACTORY_H
#define POWERFACTORY_H

#include "power.h"

class PowerFactory
{
public:
    static std::unique_ptr<Power> createPower(PowerType type, Ball* b, qreal potency);
};

#endif // POWERFACTORY_H
