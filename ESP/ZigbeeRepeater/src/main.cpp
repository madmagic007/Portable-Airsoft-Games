#include "zigbee/ZigbeeController.h"
#include "modules/AirsoftPoint.h"

static ZigbeeController zigbee;
static AirsoftPoint point(1);

void setup() {
    Serial.begin(115200);
    rgbLedWrite(RGB_BUILTIN, 1, 0, 0);
    delay(2000);

    zigbee.setup(&point);
}

void loop() {
    zigbee.loop();
}