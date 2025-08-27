#pragma once

#include "Zigbee.h"
#include "CustomCluster.h"

#define BOARD_LED RGB_BUILTIN

class ZigbeeController {
private:
    static void checkTask(void* _);

    inline static CustomCluster* _clusters = nullptr; 
    inline static bool _confirmed = false;
    
public:
    static void setup(CustomCluster* clusters, size_t size);
    static void confirmed();
    static void loop();
};