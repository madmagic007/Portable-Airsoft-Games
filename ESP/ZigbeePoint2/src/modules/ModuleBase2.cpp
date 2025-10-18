#include "ModuleBase2.h"

void ModuleBase2::zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) {
    uint8_t* bytes = static_cast<uint8_t*>(message->attribute.data.value);
    uint8_t length = bytes[0];
    
    uint8_t data[length];
    for (uint8_t i = 0; i < length; i++) {
        data[i] = bytes[i + 1];
    }
    
    if (_setup) receiveData(data, length);
}

void ModuleBase2::setPins(const uint8_t* pins) {
    _pins = pins;
}

void ModuleBase2::doSetup() {
    if (_setup) return;
    setup();
    
    _setup = setupCheck();
}

void ModuleBase2::doLoop() {
    if (_setup) loop();
}

void ModuleBase2::startTask(String name) {
    if (!_setup) return;
    
    if (_taskHandle) {            
        _stop = true;
        _taskHandle = nullptr;
    }
    
    xTaskCreate(taskWrapper, name.c_str(), 4096, this, 1, &_taskHandle);
}

void ModuleBase2::stopTask() {
    if (_taskHandle) vTaskDelete(_taskHandle);
    _taskHandle = nullptr;
}

void ModuleBase2::taskWrapper(void* pvParameters) {
    ModuleBase2* instance = static_cast<ModuleBase2*>(pvParameters);
    instance->_stop = false;
    instance->task();
    vTaskDelete(NULL);
}

void ModuleBase2::pinOutput(uint8_t pin, uint8_t defaultVal) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, defaultVal);
}