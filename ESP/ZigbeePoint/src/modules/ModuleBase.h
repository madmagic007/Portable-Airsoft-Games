#pragma once

typedef bool (*ReportCallback)(uint8_t[], void* context);

#include "Arduino.h"
#include "../Util.h"

class ModuleBase {
public:
    ModuleBase(const uint8_t* pins) : _pins(pins) {}

    // for the 2 odd one out devices
    void setPins(const uint8_t* pins) { 
        _pins = pins; 
    }

    void doSetup() {
        if (_setup) return;
        setup();
        _setup = setupCheck();
    }
    
    void doReceiveData(uint8_t arr[], size_t size) {
        if (!_setup) return;
        receiveData(arr, size);
    }

    void doLoop() {
        if (!_setup) return;
        loop();
    }

    void setReportCB(ReportCallback cb, void* context) {
        _reportCB = cb;
        _context = context;
    }
protected:
    virtual void setup() {}
    virtual bool setupCheck() { return true; }
    virtual void receiveData(uint8_t arr[], size_t size) {}
    virtual void task() {}
    virtual void loop() {}
        
    bool reportValue(const String& str) {
        if (!_reportCB) return false;
        
        uint8_t len = str.length();
        uint8_t buffer[len + 1];
        buffer[0] = len;
        memcpy(buffer + 1, str.c_str(), len);

        return _reportCB(buffer, _context);
    }

    void startTask(String name) {
        if (!_setup) return;

        if (_taskHandle) {            
            _stop = true;
            _taskHandle = nullptr;
        }

        xTaskCreate(taskWrapper, name.c_str(), 4096, this, 1, &_taskHandle);
    }

    void stopTask() {
        if (_taskHandle) vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }

    static void taskWrapper(void* pvParameters) {
        ModuleBase* instance = static_cast<ModuleBase*>(pvParameters);
        instance->_stop = false;
        instance->task();
        vTaskDelete(NULL);
    }

    void pinOutput(uint8_t pin, uint8_t defaultVal = LOW) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, defaultVal);
    }

    const uint8_t* _pins;
    bool _setup = false;
    volatile bool _stop = true;

    TaskHandle_t _taskHandle = nullptr;
    ReportCallback _reportCB = nullptr;
    void* _context = nullptr;
};