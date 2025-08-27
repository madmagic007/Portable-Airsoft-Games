#include "zigbee/ZigbeeController.h"
#include "modules/AirsoftPoint.h"
#include "modules/Scanner.h"
// #include "Buzzer.h"
// #include "P9813Driver.h"
// #include "BatteryMonitor.h"

#define PIN_BUZZER 20
#define PINS_LEDS_DRIVER 14, 15 //data, clk
const uint8_t scannerPins[] = {0, 1, 2, 3, 4, 5, 6, 7}; //sda, clk, mosi, miso, R, G, B

static ZigbeeController zigbee;
static AirsoftPoint airsoftPoint(nullptr);
static Scanner scanner(scannerPins);

static std::map<String, ModuleBase*> modules = {
    { "airsoftPoint", &airsoftPoint },
    { "scanner", &scanner }
};

static CustomCluster clusters[] = {
    { 2, "airsoftPoint", "data" , "airsoftPoint" },
    { 3, "scannedTag", "scanTagDuration", "scanner" }
    // ({ 4, "", "setColor", "" }),
    // ({ 5, "", "buzz", "" }),
    // ({ 6, "battery", "", "" })
};
static size_t clusterSize = sizeof(clusters) / sizeof(clusters[0]);

void setup() {
    Serial.begin(115200);
    delay(2000);

    airsoftPoint.set(modules, clusters, clusterSize);
    for (auto& c : clusters) {
        c.setup(modules);
    }
    
    zigbee.setup(clusters, clusterSize);
    //buzzer.setup(PIN_BUZZER);
    //led.setup(PINS_LED_DRIVER);
    //battery.setup();
    //scanner.setup(PINS_SCANNER);

    //zigbee.setCB("scanner", &scanner.callBack);
    //zigbee.setCB("led", &led.callBack);
    //zigbee.setCB("buzzer", &buzzer.callBack);

    //scanner.setReportCB(&zigbee.reportScanner);
    //battery.setReportCB(&zigbee.reportBattery);
}

void loop() {
    //zigbee.loop();
    //scanner.loop();
    //battery.loop();
}