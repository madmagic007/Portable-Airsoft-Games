#pragma once

#include "../zigbee/CustomCluster.h"

class AirsoftPoint : public CustomCluster {
public:
    using CustomCluster::CustomCluster;
    
    void setup() {
        _self = this;
        
        xTaskCreate(
            checkTask, "zigbeeCheckTask", 2048,
            NULL, 1, NULL
        );
    }

    static void confirmed() {
        if (_confirmed) return;

        rgbLedWrite(RGB_BUILTIN, 0, 1, 0);
        _confirmed = true;
    }
private:
    static void checkTask(void* _) {
        while (!_confirmed) {
            delay(3000);
            if (_confirmed) break;
            _self->sendValue("online");
        }
        
        vTaskDelete(NULL);
    }
    
    inline volatile static bool _confirmed = false;
    inline static AirsoftPoint* _self = nullptr;
};