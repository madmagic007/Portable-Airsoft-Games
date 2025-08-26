#pragma once

#include <functional>
#include "../Util.h"

class ModuleBase {
public:
    ModuleBase(const int* pins) : _pins(pins) {}
    virtual void setup();
    virtual static void callBack(uint8_t arr[], size_t size);

    void setReportCB(std::function<void(uint8_t[], String)> reportCB) {
        _reportCB = reportCB;
    }
        
    bool reportValue(uilt8_t arr[], String clusterName) {
        if (_reportCB) _reportCB(arr, clusterName);
    }
protected:
    const int* _pins;
    std::function<void(uint8_t[], String)> _reportCB;
};