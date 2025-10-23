#pragma once

#include "ModuleBase2.h"

class P9813Driver : public ModuleBase2 {
public:
    using ModuleBase2::ModuleBase2;

    void setup() override {        
        _data = _pins[0];
        _clk = _pins[1];

        pinOutput(_data);
        pinOutput(_clk, HIGH);
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

        startTask("ledDriverTask");
    }

    void task() override {
        Serial.printf("driver: %d, %d, %d, %d\n", _r, _g, _b, _dur);

        sendByte(0b11000000);
        sendByte(_b);
        sendByte(_g);
        sendByte(_r);
        latch();

        if (_dur == 0) return;
        delay(_dur);

        sendByte(0b11000000);
        sendByte(0);
        sendByte(0);
        sendByte(0);
        latch();
    }

    float getPowerFactor() {
        if (!_setup) return 0;

        float pc = 0;
        pc += _r/255;
        pc += _g/255;
        pc += _b/255;

        return pc;
    }
    
private:
    void sendBit(bool bitVal) {
        digitalWrite(_data, bitVal);
        digitalWrite(_clk, LOW);
        delayMicroseconds(1);
        digitalWrite(_clk, HIGH);
        delayMicroseconds(1);
    }

    void sendByte(uint8_t b) {
        for (int i = 7; i >= 0; i--) {
            sendBit(b & (1 << i));
        }
    }

    void latch() {
        for (int i = 0; i < 36; i++) {
            sendBit(0);
        }
    }

    uint8_t _data, _clk;
    uint8_t _r, _g, _b;
    uint16_t _dur;
};
