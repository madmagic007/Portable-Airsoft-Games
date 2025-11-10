#pragma once

#include "ModuleBase2.h"

class AirsoftPoint : public ModuleBase2 {
public:
    using ModuleBase2::ModuleBase2;
    
    void setup() override {
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
    
    void receiveData(uint8_t arr[], size_t size) override {
        String str = String(arr, size);
        
        if (str == "restart") {
            xTaskCreate(
                restartTask, "restartTask", 2048,
                NULL, 1, NULL
            );
        }
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
    
    static void restartTask(void* _) {
        delay(1000);
        ESP.restart();
        
        vTaskDelete(NULL);
    }
    
    inline volatile static bool _confirmed = false;
    inline static AirsoftPoint* _self = nullptr;
};