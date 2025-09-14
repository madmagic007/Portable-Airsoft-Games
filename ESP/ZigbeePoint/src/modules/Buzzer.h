#pragma once

#include "ModuleBase.h"

class Buzzer : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    void setup() override {
        Serial.println("setup buzzer");
        pinMode(_pins[0], OUTPUT);
        digitalWrite(_pins[0], LOW);
    }

    void receiveData(uint8_t arr[], size_t size) override {
        char buf[size + 1];
        memcpy(buf, arr, size);
        buf[size] = '\0';
        
        _buzzDuration = atof(buf);
        
        startTask("buzzerTask");
    }

    void task() override {
        digitalWrite(_pins[0], HIGH);
        delay(seconds * 1000);
        digitalWrite(_pins[0], LOW);
    }
private:
    float _buzzDuration = 0;
};