#pragma once

#include "Zigbee.h"
#include "../modules/AirsoftPoint.h"

class ZigbeeController {
public:
    static void setup(AirsoftPoint* ap);
    static void loop();
};