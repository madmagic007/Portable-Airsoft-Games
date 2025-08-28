#pragma once

typedef bool (*ReportCallback)(uint8_t[], void* context);

#include "Arduino.h"
#include "../Util.h"

class ModuleBase {
public:
    ModuleBase(const uint8_t* pins) : _pins(pins) {}
    virtual void callBack(uint8_t arr[], size_t size) {}

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
        
    bool reportValue(uint8_t arr[]) {
        if (_reportCB) return _reportCB(arr, _context);
        return false;
    }
protected:
    struct TaskData {
        const uint8_t* pins;
        uint8_t* taskArr;
        size_t size;
    };

    virtual void setup() {}
    virtual void loop() {}

    const uint8_t* _pins;
    bool _setup = false;

    ReportCallback _reportCB = nullptr;
    void* _context = nullptr;
};