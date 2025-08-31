#pragma once

#include "ModuleBase.h"

class GenericLed : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    void setup() override {
        Serial.println("generic led setup");
        _pinR = _pins[0];
        _pinG = _pins[1];
        _pinB = _pins[2];

        pinOutput(_pinR);
        pinOutput(_pinG);
        pinOutput(_pinB);
    }

    void receiveData(uint8_t arr[], size_t size) override {
        if (size < 6) return;
        
        uint8_t cols[3];
        Util::hexStringToBytes(arr, cols, 6);

        _r = cols[0];
        _g = cols[1];
        _b = cols[2];
        _dur = 0;

        if (size > 6) {
            for (size_t i = 6; i < size; ++i) {
                char c = arr[i];
                _dur = _dur * 10 + (c - '0');
            }
        }
        
        startTask("genericLedTask");
    }

    void task() override {
        Serial.printf("%d, %d, %d, %d\n", _r, _g, _b, _dur);

        analogWrite(_pinR, _r);
        analogWrite(_pinG, _g);
        analogWrite(_pinB, _b);

        if (_dur == 0) return;
        vTaskDelay(pdMS_TO_TICKS(_dur));

        analogWrite(_pinR, 0);
        analogWrite(_pinG, 0);
        analogWrite(_pinB, 0);
    }
private:
    uint8_t _pinR, _pinG, _pinB;
    uint8_t _r, _g, _b;
    uint16_t _dur;
};