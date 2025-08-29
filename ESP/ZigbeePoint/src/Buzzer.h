#pragma once

#include "Arduino.h"

class Buzzer {
    
public:
    void setup(int buzzer) {
        pinMode(buzzer, OUTPUT);
        digitalWrite(buzzer, LOW);

        _buzzer = buzzer;
    }
    
    static void callBack(uint8_t arr[], size_t size) {
        
    }
    
private:
    static void buzzerTask(void* pvParameters) {
        int val = (int)pvParameters;
        
        digitalWrite(_buzzer, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000 * val));
        digitalWrite(_buzzer, LOW);

        vTaskDelete(NULL);
    }

    inline static int _buzzer = -1;
};