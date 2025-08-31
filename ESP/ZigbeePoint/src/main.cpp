#include "zigbee/ZigbeeController.h"
#include "modules/AirsoftPoint.h"
#include "modules/Scanner.h"
#include "modules/P9813Driver.h"
#include "modules/Buzzer.h"
#include "modules/BatteryMonitor.h"
#include "modules/GenericLed.h"

//const uint8_t scannerPins[] = {0, 1, 2, 3, 4, 5, 6, 7}; // sda, clk, mosi, miso, rst, R, G, B
const uint8_t scannerPins[] = {4, 5, 6, 7, 3, 0, 1, 2}; // sda, clk, mosi, miso, rst, R, G, B
const uint8_t ledDriverPins[] = {14, 15}; // data clk
const uint8_t buzzerPins[] = {20};
const uint8_t batteryPins[] = {21};
const uint8_t genericLedPins[] = {21, 22, 23}; // R, G, B

static ZigbeeController zigbee;
static AirsoftPoint airsoftPoint(nullptr);
static Scanner scanner(scannerPins);
static P9813Driver ledDriver(ledDriverPins);
static Buzzer buzzer(buzzerPins);
static BatteryMonitor batteryMonitor(batteryPins);
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
    { 2, "airsoftPoint", "data" , "airsoftPoint" }, // x
    { 3, "scannedTag", "scanTagDuration", "scanner" }, // 0
    { 4, "", "setDriverColor", "ledDriver" }, // 1
    { 5, "", "buzz", "buzzer"}, // 2
    { 6, "battery", "", "battery" }, // 3
    { 7, "", "setGenericColor", "genericLed" } // 4
};
static size_t clusterSize = sizeof(clusters) / sizeof(clusters[0]);

void setup() {
    rgbLedWrite(BOARD_LED, 1, 0, 0);
    Serial.begin(115200);
    delay(2000);

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