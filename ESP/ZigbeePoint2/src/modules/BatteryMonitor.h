// #pragma once

// #include "ModuleBase2.h"
// #include "P9813Driver.h"
// #include "Preferences.h"

// #define SAMPLES 20.0
// #define BATTER_REPORT_INTERVAL 2 //seconds

// class BatteryMonitor : public ModuleBase2 {
// public:
//     BatteryMonitor(const uint8_t* pins, P9813Driver* ledDriver) : ModuleBase2(pins) {
//         _ledDriver = ledDriver;
//     }

//     static void initStorage(void* pvParameters) {
        
//     }

//     bool setupCheck() override {
//         if (_pins[0] == 21) {
//             Serial.println("Ignoring battery monitor, wrong device");
//             return false;
//         }

//         prefs.begin("batterySettings", false);
//         _vMin = prefs.getFloat("vMin", 11.0);
//         _vMax = prefs.getFloat("vMax", 12.75);
//         _divRatio = prefs.getFloat("divRatio", 0.1818);
//         _colorVd = prefs.getFloat("colorVd", 0.5);

//         return true;
//     }

//     void loop() {
//         long elapsed = millis() - _lastReport;
//         if (elapsed < 1000 * BATTER_REPORT_INTERVAL) return;
//         _lastReport = millis();

//         startTask("batteryMonitor");
//     }

//     void task() override {
//         //dummy read
//         analogRead(_pins[0]);
//         vTaskDelay(pdMS_TO_TICKS(10));
        
//         long sum = 0;
//         for (int i=0; i < SAMPLES; i++){
//             sum += analogReadMilliVolts(_pins[0]);
//             vTaskDelay(pdMS_TO_TICKS(10));
//         }

//         float vAdc = sum / SAMPLES / 1000;
//         float vBatRaw = vAdc / _divRatio;

//         float colF = _ledDriver->getPowerFactor();
//         float vdTotal = _vdIdle + colF * _colorVd;

//         float vBat = min(vBatRaw + vdTotal, _vMax);

//         float range = _vMax - _vMin;
//         float left = vBat - _vMin;
//         float percentage = min(left / range * 100.0f, 100.0f);
        
//         String str = String(vBatRaw, 2) + "|" + String(vBat, 2) + "|" + String(percentage, 1);
//         reportValue(str);
//     }

//     void receiveData(uint8_t arr[], size_t size) override {
//         char buf[size + 1];
//         memcpy(buf, arr, size);
//         buf[size] = '\0';

//         sscanf(buf, "%f|%f|%f|%f", &_vMin, &_vMax, &_divRatio, &_colorVd);

//         prefs.begin("batterySettings", false);
//         prefs.putFloat("vMin", _vMin);
//         prefs.putFloat("vMax", _vMax);
//         prefs.putFloat("divRatio", _divRatio);
//         prefs.putFloat("colorVd", _colorVd);
//     }
// private:
//     float _vMin, _vMax, _divRatio, _colorVd;
//     float _vdIdle = 0.05;

//     Preferences prefs;
//     P9813Driver* _ledDriver;
//     volatile long _lastReport = millis();
// };