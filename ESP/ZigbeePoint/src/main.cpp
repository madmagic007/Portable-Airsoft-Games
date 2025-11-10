#include "zigbee/ZigbeeController.h"
#include "modules/AirsoftPoint.h"
#include "modules/Scanner.h"
#include "modules/P9813Driver.h"
#include "modules/Buzzer.h"
#include "modules/GenericLed.h"

const uint8_t buzzerPins[] = {20};
const uint8_t scannerPins[] = {2, 1, 0, buzzerPins[0]}; // R, G, B, buzzer
const uint8_t ledDriverPins[] = {15, 14}; // data, clk
const uint8_t genericLedPins[] = {14, 15, 20}; // R, G, B

static ZigbeeController zigbee;
static AirsoftPoint airsoftPoint(nullptr, 1, true, true);
static Scanner scanner(scannerPins, 2, true, true);
static P9813Driver ledDriver(ledDriverPins, 3, false, true);
static Buzzer buzzer(buzzerPins, 4, false, true);
static GenericLed genericLed(genericLedPins, 5, false, true);

static ModuleBase2* modules[] = {
    &airsoftPoint, &scanner, &ledDriver, &buzzer, &genericLed
};
static size_t modulesSize = sizeof(modules) / sizeof(modules[0]);

void setup() {
    Serial.begin(115200);
    rgbLedWrite(RGB_BUILTIN, 1, 0, 0);
    delay(2000);

    // airsoftPoint get setup in zigbeeController because it polls z2m
    for (size_t i = 1; i < modulesSize; i++) {
        modules[i]->doSetup();
    }
    
    zigbee.setup(modules, modulesSize);
}

void loop() {
    zigbee.loop();
    
    for (size_t i = 0; i < modulesSize; i++) {
        modules[i]->doLoop();
    }
}