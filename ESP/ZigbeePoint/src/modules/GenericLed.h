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
        char buf[size + 1];
        memcpy(buf, arr, size);
        buf[size] = '\0';
        
        uint8_t cols[3];
        uint8_t hexStr[6];
        _blinkCount = 0;
        _onDuration = 0;
        _pause = 0;

        int n = sscanf(buf, "%6s|%f|%hhu|%f", hexStr, &_onDuration, &_blinkCount, &_pause);
        if (n == 0 || n == 3) return;

        Util::hexStringToBytes(arr, cols, 6);
        _r = cols[0];
        _g = cols[1];
        _b = cols[2];
        
        startTask("genericLedTask");
    }

    void task() override {
        uint8_t i = 0;
        do {
            writeCol(_r, _g, _b);

            if (_onDuration == 0) return;

            delay(_onDuration * 1000);
            writeCol(0, 0, 0);
            delay(_pause * 1000);

            i++;
        } while ((i < _blinkCount || _blinkCount == 255) && !_stop);
    }
private:
    uint8_t _pinR, _pinG, _pinB;
    uint8_t _r, _g, _b, _blinkCount;
    float _onDuration, _pause;

    void writeCol(uint8_t r, uint8_t g, uint8_t b) {
        analogWrite(_pinR, r);
        analogWrite(_pinG, g);
        analogWrite(_pinB, b);
    }
};