#pragma once

#include "ModuleBase.h"

class Buzzer : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    void setup() override {
        pinMode(_pins[0], OUTPUT);
        digitalWrite(_pins[0], LOW);
    }

    void receiveData(uint8_t arr[], size_t size) override {
        char buf[size + 1];
        memcpy(buf, arr, size);
        buf[size] = '\0';
        int value = atoi(buf);
        
        xTaskCreate(
            buzzerTask, "buzzerTask", 2048,
             (void*)value, 1, NULL
        );
    }
};