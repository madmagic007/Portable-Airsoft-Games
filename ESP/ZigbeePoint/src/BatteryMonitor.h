#pragma once
#include <Arduino.h>

#define PIN_BATTERY 35
#define R1 295000.0
#define R2 65000.0
#define SAMPLES 20.0
#define V_MIN 12.0
#define V_MAX 12.85
#define BATTER_REPORT_INTERVAL 2 //seconds

#define DIV_RATIO float(R2 / (R1 + R2))
#define VREF 3.3
#define ADC_RANGE 4095

class BatteryMonitor {
public:
    static void setup() {
        analogSetPinAttenuation(PIN_BATTERY, ADC_11db);
    }

    static void readBattery(float* percentage, float *voltage) {
        // long sum = 0;
        
        // for (int i=0; i < SAMPLES; i++){
        //     sum += analogRead(PIN_BATTERY);
        //     delay(5);
        // }

        // float adc = sum / SAMPLES;
        // float vadc = adc / ADC_RANGE * VREF;
        // float vBat = vadc / DIV_RATIO;
        float vBat = 12.48;
        *voltage = roundf(vBat * 100.0f) / 100.0f;

        float range = V_MAX - V_MIN;
        float left = vBat - V_MIN;
        *percentage = roundf(left / range * 1000.0f) / 10.0f;
    }

    static void readPrint() {
        float percentage, voltage;
        readBattery(&percentage, &voltage);

        Serial.printf("%d% (%dV)\n", percentage, voltage);
    }

    static void loop() {
        long elapsed = millis() - lastReport;
        if (elapsed < 1000 * BATTER_REPORT_INTERVAL | !batteryCB) return;
        lastReport = millis();

        float percentage, voltage;
        readBattery(&percentage, &voltage);

        int scaledPercentage = percentage * 10;
        int scaledVoltage = (voltage - 12) * 100;
        float packed = ((uint16_t)scaledPercentage << 8) | scaledVoltage;

        batteryCB(packed);
    }

    static void setReportCB(void (*callback)(float)) {
        batteryCB = callback;
    }

private:
    inline static void (*batteryCB)(float) = nullptr;
    inline static long lastReport = millis();
};