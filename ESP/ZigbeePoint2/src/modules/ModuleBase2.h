#pragma once

#include "Arduino.h"
#include "ZigbeeEP.h"
#include "../Util.h"
#include "../zigbee/CustomCluster.h"

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000

class ModuleBase2 : public CustomCluster {
public:
    ModuleBase2(const uint8_t* pins, uint8_t endpoint) : CustomCluster(endpoint), _pins(pins) {}
    void setPins(const uint8_t* pins);
    void doSetup();
    void doLoop();
    virtual void receiveData(uint8_t arr[], size_t size) {};

private:
    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) override;
    static void taskWrapper(void* pvParameters);

    TaskHandle_t _taskHandle = nullptr;

protected:
    virtual void setup() {};
    virtual void loop() {};
    virtual bool setupCheck() { return true; };
    virtual void task() {};
    void startTask(String name);
    void stopTask();
    void pinOutput(uint8_t pin, uint8_t defaultVal = LOW);

    const uint8_t *_pins;
    volatile bool _stop = true;
    bool _setup = false;
};
