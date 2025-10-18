#pragma once

#include "Zigbee.h"
#include "../modules/ModuleBase2.h"

class ZigbeeController {
public:
    static void setup(ModuleBase2* clusters[], size_t size);
    static void loop();
};