#pragma once

#include "Zigbee.h"
#include "CustomCluster.h"

#define BOARD_LED RGB_BUILTIN

class ZigbeeController {
private:
    static void checkTask(void* _);
    
    inline static bool _confirmed = false;
    
public:
    static void setup();
    static void confirmed();
    static void loop();
};