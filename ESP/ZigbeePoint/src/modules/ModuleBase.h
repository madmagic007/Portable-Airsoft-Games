#pragma once

typedef bool (*ReportCallback)(uint8_t[], void* context);
typedef bool (*ReceiveCallback)(uint8_t[], size_t, void* context);

#include "Arduino.h"
#include "../Util.h"

class ModuleBase {
public:
    ModuleBase(const uint8_t* pins) : _pins(pins) {}
    virtual void setup() = 0;
    virtual void callBack(uint8_t arr[], size_t size) = 0;

    void setReportCB(ReportCallback cb, void* context) {
        _reportCB = cb;
        _context = context;
    }
        
    bool reportValue(uint8_t arr[]) {
        if (_reportCB) return _reportCB(arr, _context);
        return false;
    }
protected:
    const uint8_t* _pins;

    ReportCallback _reportCB = nullptr;
    void* _context = nullptr;
};