#pragma once

#include "ModuleBase.h"

class P9813Driver : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    void setup() override {
        pinMode(_pins[0], OUTPUT); // clk
        pinMode(_pins[1], OUTPUT); // data

        digitalWrite(_pins[0], LOW);
        digitalWrite(_pins[1], LOW);
    }

    void receiveData(uint8_t arr[], size_t size) override {
        if (size < 6 || size % 2 != 0) return;
        
        _taskArr = new uint8_t[size / 2];
        CustomCluster::hexStringToBytes(arr, _taskArr, size);

        startTask("ledTask");
    }

    void task() override {
        byte r = _taskArr[0];
        byte g = _taskArr[1];
        byte b = _taskArr[2];

        sendByte(0b11000000);
        sendByte(b);
        sendByte(g);
        sendByte(r);
        latch();

        Serial.printf("%d, %d, %d\n", r, g, b);

        delete[] _taskArr;
        _taskArr = nullptr;
    }

private:
    uint8_t* _taskArr = nullptr;

    void sendBit(bool bitVal) {
        digitalWrite(_pins[1], bitVal); // data
        digitalWrite(_pins[0], LOW);    // clk
        delayMicroseconds(1);
        digitalWrite(_pins[0], HIGH);   // clk
        delayMicroseconds(1);
    }

    void sendByte(byte b) {
        for (int i = 7; i >= 0; i--) {
            sendBit(b & (1 << i));
        }
    }

    void latch() {
        for (int i = 0; i < 36; i++) {
            sendBit(0);
        }
    }
};
