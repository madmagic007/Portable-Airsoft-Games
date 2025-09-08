#pragma once

#include "ModuleBase.h"

#define R1 295000.0
#define R2 65000.0
#define SAMPLES 20.0
#define V_MIN 12.0
#define V_MAX 12.85
#define BATTER_REPORT_INTERVAL 2 //seconds

#define DIV_RATIO float(R2 / (R1 + R2))
#define VREF 3.3
#define ADC_RANGE 4095

class BatteryMonitor : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    bool setupCheck() override {
        if (_pins[0] == 21) {
        Serial.println("Ignoring battery monitor, wrong device");
            return false;
        }

        analogSetPinAttenuation(_pins[0], ADC_11db);

        return true;
    }

    void loop() {
        long elapsed = millis() - _lastReport;
        if (elapsed < 1000 * BATTER_REPORT_INTERVAL) return;
        _lastReport = millis();

        startTask("batteryMonitor");
    }

    void task() override {
        long sum = 0;

        Serial.print("reading from: ");
        Serial.println(_pins[0]);
        
        for (int i=0; i < SAMPLES; i++){
            sum += analogRead(_pins[0]);
            delay(5);
        }

        float adc = sum / SAMPLES;
        float vadc = adc / ADC_RANGE * VREF;
        float vBat = vadc / DIV_RATIO;

        float range = V_MAX - V_MIN;
        float left = vBat - V_MIN;
        float percentage = left / range * 100.0f;

        String str = String(vBat, 2) + "|" + String(percentage, 0);
        reportValue(str);
    }
private:
    volatile long _lastReport = millis();
};