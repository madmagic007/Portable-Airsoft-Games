#pragma once

#include "Zigbee.h"
#include "CustomCluster.h"
#include "Arduino.h"
#include <map>

#define BOARD_LED RGB_BUILTIN

class ZigbeeController {
private:
    static void setupTask(void* _);
    static void checkTask(void* _);
    static void confirmed(uint8_t[], size_t);

    inline static bool _confirmed = false;
    
public:
    inline static CustomCluster clusterList[] = {
        CustomCluster({ 2, "airsoftPointOnline", "confirmed" }),
        CustomCluster({ 3, "scannedTag", "scanTagDuration" }),
        CustomCluster({ 4, "", "setColor" }),
        CustomCluster({ 5, "", "buzz" }),
        CustomCluster({ 6, "battery", "" })
    };
    inline static size_t clusterSize = sizeof(clusterList) / sizeof(clusterList[0]);

    inline static std::map<String, CustomCluster*> clusters = {
        { "airsoftPoint", &clusterList[0] },
        { "scanner",      &clusterList[1] },
        { "led",          &clusterList[2] },
        { "buzzer",       &clusterList[3] },
        { "battery",      &clusterList[4] }
    };

    void setup() {
        xTaskCreate(
            setupTask, "zigbeeSetupTask", 2048,
            NULL, 1, NULL
        );
    }

    static void reportValue(uint8_t arr[], String clusterName);
    
    void setCB(String clusterName, void (*callback)(uint8_t[], size_t));

    void loop();
};