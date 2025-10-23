#include "ZigbeeController.h"

void ZigbeeController::setup(AirsoftPoint* ap) {
    pinMode(BOOT_PIN, INPUT_PULLUP);

    ZigbeeRangeExtender extender(2);
    extender.setManufacturerAndModel("MadMagic", "AirsoftPoint");
    extender.setPowerSource(ZB_POWER_SOURCE_MAINS);
    Zigbee.addEndpoint(&extender);

    Zigbee.addEndpoint(ap);

    Zigbee.setTimeout(INT32_MAX);
    if (!Zigbee.begin(ZIGBEE_ROUTER)) {
        Serial.println("Zigbee failed to start!");
        Serial.println("Rebooting...");
        ESP.restart();
    }
    rgbLedWrite(RGB_BUILTIN, 10, 1, 0);

    while (!Zigbee.connected()) delay(100);
    rgbLedWrite(RGB_BUILTIN, 0, 0, 1);

    delay(1000);
    ap->setup();
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