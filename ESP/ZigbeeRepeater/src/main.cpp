#include "zigbee/ZigbeeController.h"

static ZigbeeController zigbee;

void setup() {
    Serial.begin(115200);
    rgbLedWrite(BOARD_LED, 1, 0, 0);
    delay(2000);

    zigbee.setup();
}

void loop() {
    zigbee.loop();
}