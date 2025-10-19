#include "ZigbeeController.h"

void ZigbeeController::setup(CustomCluster* clusters, size_t size) {
    _clusters = clusters;

    pinMode(BOOT_PIN, INPUT_PULLUP);

    ZigbeeRangeExtender extender(1);
    extender.setManufacturerAndModel("MadMagic", "AirsoftPoint");
    extender.setPowerSource(ZB_POWER_SOURCE_MAINS);
    Zigbee.addEndpoint(&extender);

    for (size_t i = 0; i < size; i++) {
        Zigbee.addEndpoint(&clusters[i]);
    }

    Zigbee.setTimeout(INT32_MAX);
    if (!Zigbee.begin(ZIGBEE_ROUTER)) {
        Serial.println("Zigbee failed to start!");
        Serial.println("Rebooting...");
        ESP.restart();
    }
    rgbLedWrite(BOARD_LED, 10, 1, 0);

    while (!Zigbee.connected()) vTaskDelay(pdMS_TO_TICKS(100));

    rgbLedWrite(BOARD_LED, 0, 0, 1);

    xTaskCreate(
        checkTask, "zigbeeCheckTask", 2048,
        NULL, 1, NULL
    );
}

void ZigbeeController::checkTask(void* _) {
    while (!_confirmed) {
        _clusters[0].reportAttribs();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void ZigbeeController::confirmed() {
    if (_confirmed) return;
    _confirmed = true;

    rgbLedWrite(RGB_BUILTIN, 0, 1, 0);
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