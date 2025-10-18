#include "zigbee/ZigbeeController.h"
#include "modules/AirsoftPoint.h"
#include "modules/Scanner.h"
#include "modules/P9813Driver.h"
#include "modules/Buzzer.h"
#include "modules/BatteryMonitor.h"
#include "modules/GenericLed.h"

const uint8_t scannerPins[] = {4, 5, 6, 7, 23, 1, 2, 3, 20}; // sda, clk, mosi, miso, rst, R, G, B, buzzer
const uint8_t ledDriverPins[] = {15, 14}; // data clk
const uint8_t buzzerPins[] = {20};
const uint8_t batteryPins[] = {0};
const uint8_t genericLedPins[] = {14, 15, 20}; // R, G, B

// faulty one, 2 use this layout
const uint8_t scanner1Pins[] = {3, 4, 5, 6, 7, 0, 1, 2}; // sda, clk, mosi, miso, rst, R, G, B
const uint8_t battery1Pins[] = {21};

static ZigbeeController zigbee;
static AirsoftPoint airsoftPoint(nullptr, 2, "airsoftPoint", "data");
static Scanner scanner(scannerPins, 3, "scannedTag", "scannerSettings");
static P9813Driver ledDriver(ledDriverPins, 4, "", "setDriverColor");
static Buzzer buzzer(buzzerPins, 5, "", "buzz");
//static BatteryMonitor batteryMonitor(batteryPins, &ledDriver); //ugh
static GenericLed genericLed(genericLedPins, 7, "", "setGenericColor");

static ModuleBase2* modules[] = {
    &airsoftPoint, &scanner, &ledDriver, &buzzer, &genericLed
};
static size_t modulesSize = sizeof(modules) / sizeof(modules[0]);

void setup() {
    Serial.begin(115200);
    rgbLedWrite(RGB_BUILTIN, 1, 0, 0);
    delay(2000);

    if (Util::checkPinDischarge(battery1Pins[0]) < 100) {
        scanner.setPins(scanner1Pins);
        //batteryMonitor.setPins(battery1Pins);
    
    }

    // airsoftPoint get setup in zigbeeController because it polls z2m
    for (size_t i = 1; i < modulesSize; i++) {
        modules[i]->doSetup();
    }
    
    zigbee.setup(modules, modulesSize);

    uint8_t data[] = {'0'};
    scanner.receiveData(data, 1);
}

void loop() {
    zigbee.loop();
    
    for (size_t i = 0; i < modulesSize; i++) {
        modules[i]->doLoop();
    }
}