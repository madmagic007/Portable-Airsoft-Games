#include "ZigbeeController.h"

void ZigbeeController::setup(ModuleBase2* modules[], size_t size) {
    pinMode(BOOT_PIN, INPUT_PULLUP);

    ZigbeeRangeExtender extender(1);
    extender.setManufacturerAndModel("MadMagic", "AirsoftPoint");
    extender.setPowerSource(ZB_POWER_SOURCE_MAINS);
    Zigbee.addEndpoint(&extender);

    for (size_t i = 0; i < size; i++) {
        Zigbee.addEndpoint(modules[i]);
    }

    Zigbee.setTimeout(INT32_MAX);
    if (!Zigbee.begin(ZIGBEE_ROUTER)) {
        Serial.println("Zigbee failed to start!");
        Serial.println("Rebooting...");
        ESP.restart();
    }
    rgbLedWrite(RGB_BUILTIN, 10, 1, 0);

    while (!Zigbee.connected()) vTaskDelay(pdMS_TO_TICKS(100));

    rgbLedWrite(RGB_BUILTIN, 0, 0, 1);
    modules[0]->doSetup();
}

void ZigbeeController::loop() {
    if (digitalRead(BOOT_PIN) != LOW) return;
    delay(100);

    int startTime = millis();
    while (digitalRead(BOOT_PIN) == LOW) {
        delay(50);

        if ((millis() - startTime) > 3000) {
            Serial.println("resetting");
            Zigbee.factoryReset();
        }
    }
}