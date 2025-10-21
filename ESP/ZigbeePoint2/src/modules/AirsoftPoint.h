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
            _self->sendValue("online");
            vTaskDelay(pdMS_TO_TICKS(3000));
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