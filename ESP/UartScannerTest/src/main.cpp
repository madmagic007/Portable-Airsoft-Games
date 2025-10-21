#include "MFRC522.h"

// {3, 4, 5, 6, 7, 0, 1, 2}; // sda, clk, mosi, miso, rst, R, G, B
MFRC522 scanner;
HardwareSerial mySerial(1);
bool run = false;

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 6, 3);

    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
    delay(2000);

    scanner.begin(&mySerial);
    run = true;
}

void loop() {
    //Serial.println(mySerial.available());
    if (scanner.available() && run) {
        scanner.readCardSerial();
        
        Serial.println("card!");
        scanner.wait();
    }
}