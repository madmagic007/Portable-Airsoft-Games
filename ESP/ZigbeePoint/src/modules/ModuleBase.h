#pragma once

typedef bool (*ReportCallback)(uint8_t[], void* context);

#include "Arduino.h"
#include "../Util.h"

class ModuleBase {
public:
    ModuleBase(const uint8_t* pins) : _pins(pins) {}
    virtual void receiveData(uint8_t arr[], size_t size) {}

    void doSetup() {
        _setup = true;
        setup();
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
    virtual void loop() {}
    virtual void task() {}
        
    bool reportValue(const String& str) {
        if (!_reportCB) return false;
        
        uint8_t len = str.length();
        uint8_t buffer[len + 1]
        buffer[0] = len;
        memcpy(buffer + 1, str.c_str(), len);

        return _reportCB(buffer, _context);
    }

    void startTask(String name) {
        xTaskCreate(taskWrapper, name.c_str(), 4096, this, 1, &_taskHandle);
    }

    void stopTask() {
        if (_taskHandle) vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }

    static void taskWrapper(void* pvParameters) {
        ModuleBase* instance = static_cast<ModuleBase*>(pvParameters);
        instance->task();
        stopTask();
    }

    const uint8_t* _pins;
    bool _setup = false;

    TaskHandle_t _taskHandle = nullptr;
    ReportCallback _reportCB = nullptr;
    void* _context = nullptr;
};