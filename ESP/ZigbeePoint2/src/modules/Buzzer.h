#pragma once

#include "ModuleBase2.h"

class Buzzer : public ModuleBase2 {
public:
    using ModuleBase2::ModuleBase2;

    void setup() override {
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
        delay(_buzzDuration * 1000);
        digitalWrite(_pins[0], LOW);
    }
private:
    float _buzzDuration = 0;
};