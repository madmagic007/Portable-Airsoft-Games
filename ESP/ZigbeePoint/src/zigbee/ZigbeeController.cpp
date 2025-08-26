#include "ZigbeeController.h"

void ZigbeeController::setupTask(void* _) {
    pinMode(BOOT_PIN, INPUT_PULLUP);
    rgbLedWrite(BOARD_LED, 1, 0, 0);

    ZigbeeRangeExtender extender(1);
    extender.setManufacturerAndModel("MadMagic", "AirsoftPoint");
    Zigbee.addEndpoint(&extender);

    for (size_t i = 0; i < clusterSize; i++) {
        Zigbee.addEndpoint(&clusterList[i]);
    }

    clusters["airsoftPoint"]->setReceiveCallback(&confirmed);

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

    vTaskDelete(NULL);
}

void ZigbeeController::checkTask(void* _) {
    while (!_confirmed) {
        clusters["airsoftPoint"]->reportAttribs();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void ZigbeeController::confirmed(uint8_t[], size_t) {
    if (_confirmed) return;

    _confirmed = true;

    for (size_t i = 0; i < clusterSize; i++) {
        clusterList[i].reportAttribs();
    }

    rgbLedWrite(RGB_BUILTIN, 0, 1, 0);
}

void ZigbeeController::reportValue(uint8_t arr[], String clusterName) {
    clusters[clusterName]->sendValue(arr);
}

void ZigbeeController::setCB(String clusterName, void (*callback)(uint8_t[], size_t)) {
    clusters[clusterName]->setReceiveCallback(callback);
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