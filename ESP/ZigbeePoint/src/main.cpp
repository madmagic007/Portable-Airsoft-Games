#include <Arduino.h>

#include "zigbee/ZigbeeController.h"
// #include "Buzzer.h"
// #include "P9813Driver.h"
// #include "BatteryMonitor.h"
#include "Scanner.h"

ZigbeeController zigbee;
// Buzzer buzzer;
// P9813Driver led;
//BatteryMonitor battery;
Scanner scanner;

#define PIN_BUZZER 20
#define PINS_LEDS_DRIVER 14, 15 //data, clk
#define PINS_SCANNER 0, 1, 2, 3, 4, 5, 6, 7 //sda, clk, mosi, miso, R, G, B

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    zigbee.setup();
    //buzzer.setup(PIN_BUZZER);
    //led.setup(PINS_LED_DRIVER);
    //battery.setup();
    scanner.setup(PINS_SCANNER);

    zigbee.setCB("scanner", &scanner.callBack);
    //zigbee.setCB("led", &led.callBack);
    //zigbee.setCB("buzzer", &buzzer.callBack);

    scanner.setReportCB(&zigbee.reportScanner);
    //battery.setReportCB(&zigbee.reportBattery);
}

void loop() {
    zigbee.loop();
    scanner.loop();
    //battery.loop();
}