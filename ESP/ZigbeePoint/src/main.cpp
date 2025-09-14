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
static AirsoftPoint airsoftPoint(nullptr);
static Scanner scanner(scannerPins);
static P9813Driver ledDriver(ledDriverPins);
static Buzzer buzzer(buzzerPins);
static BatteryMonitor batteryMonitor(batteryPins, &ledDriver);
static GenericLed genericLed(genericLedPins);

static std::map<String, ModuleBase*> modules = {
    { "airsoftPoint", &airsoftPoint },
    { "scanner", &scanner },
    { "ledDriver", &ledDriver },
    { "buzzer", &buzzer },
    { "battery", &batteryMonitor },
    { "genericLed", &genericLed },
};

static CustomCluster clusters[] = {
    { 2, "airsoftPoint", "data" , "airsoftPoint" },
    { 3, "scannedTag", "scanTagDuration", "scanner" },  // 10000
    { 4, "", "setDriverColor", "ledDriver" },           // 01000
    { 5, "", "buzz", "buzzer"},                         // 00100
    { 6, "battery", "batterySettings", "battery" },     // 00010
    { 7, "", "setGenericColor", "genericLed" }          // 00001
};
static size_t clusterSize = sizeof(clusters) / sizeof(clusters[0]);

uint32_t checkPinDischarge(uint8_t pin) {
    uint32_t sum = 0;

    for (int i = 0; i < 5; i++) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
        delayMicroseconds(5);

        pinMode(pin, INPUT);
        uint32_t t0 = micros();
        while (digitalRead(pin) == HIGH) {
            if ((micros() - t0) > 200) break;
        }
        sum += (micros() - t0);

        delay(2);
    }

    return sum / 5;
}
void setup() {
    Serial.begin(115200);
    rgbLedWrite(BOARD_LED, 1, 0, 0);
    delay(2000);

    // check for wrong wired device
    uint32_t dt = checkPinDischarge(battery1Pins[0]);
    if (dt < 100) {
        scanner.setPins(scanner1Pins);
        batteryMonitor.setPins(battery1Pins);
        Serial.println("odd device");
    }
        Serial.println("passed");

    airsoftPoint.set(modules, clusters, clusterSize);
    for (CustomCluster& c : clusters) {
        c.setup(modules);
    }
    
    zigbee.setup(clusters, clusterSize);
}

void loop() {
    zigbee.loop();
    
    for (auto& pair : modules) {
        ModuleBase* module = pair.second;
        module->doLoop();
    }
}