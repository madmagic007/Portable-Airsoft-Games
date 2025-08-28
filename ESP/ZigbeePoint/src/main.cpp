#include "zigbee/ZigbeeController.h"
#include "modules/AirsoftPoint.h"
#include "modules/Scanner.h"
#include "modules/P9813Driver.h"
// #include "Buzzer.h"
// #include "P9813Driver.h"
// #include "BatteryMonitor.h"

#define PIN_BUZZER 20
const uint8_t scannerPins[] = {0, 1, 2, 3, 4, 5, 6, 7}; // sda, clk, mosi, miso, rst, R, G, B
const uint8_t ledDriverPins[] = {14, 15}; // clk data

static ZigbeeController zigbee;
static AirsoftPoint airsoftPoint(nullptr);
static Scanner scanner(scannerPins);
static P9813Driver ledDriver(ledDriverPins);

static std::map<String, ModuleBase*> modules = {
    { "airsoftPoint", &airsoftPoint },
    { "scanner", &scanner }
    { "ledDriver", &scanner }
};

static CustomCluster clusters[] = {
    { 2, "airsoftPoint", "data" , "airsoftPoint" },
    { 3, "scannedTag", "scanTagDuration", "scanner" },
    { 4, "", "setColor", "ledDriver" }
    // ({ 5, "", "buzz", "" }),
    // ({ 6, "battery", "", "" })
};
static size_t clusterSize = sizeof(clusters) / sizeof(clusters[0]);

void setup() {
    Serial.begin(115200);
    delay(2000);

    airsoftPoint.set(modules, clusters, clusterSize);
    for (CustomCluster& c : clusters) {
        c.setup(modules);
    }
    
    zigbee.setup(clusters, clusterSize);
    //buzzer.setup(PIN_BUZZER);
    //led.setup(PINS_LED_DRIVER);
    //battery.setup();
    //scanner.setup(PINS_SCANNER);
}

void loop() {
    zigbee.loop();
    
    for (auto& pair : modules) {
        ModuleBase* module = pair.second;
        module->doLoop();
    }
}