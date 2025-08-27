#pragma once

#include "ModuleBase.h"

class Scanner : public ModuleBase {
public:
    Scanner(const uint8_t* pins) : ModuleBase(pins) {};

    void setup() override {
        Serial.println(_pins[2]);
    }

    void callBack(uint8_t arr[], size_t size) override {

    }
};