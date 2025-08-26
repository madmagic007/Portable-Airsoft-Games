#pragma once

#include "MFRC522.h"

MFRC522 _mfrc522;

class Scanner {
public:
    static void setup(int sda, int clk, int mosi, int miso, int rst, int r, int g, int b) {
        SPI.begin(clk, miso, mosi);
        _mfrc522.PCD_Init(sda, 22); //22 is just a random number because rst does NOT work here

        _r = r;
        _g = g;
        _b = b;
        _rst = rst;

        pinMode(r, OUTPUT);
        pinMode(g, OUTPUT);
        pinMode(b, OUTPUT);
        pinMode(rst, OUTPUT);
        digitalWrite(rst, HIGH);

        xTaskCreate(task, "cardScanner", 2048, NULL, 1, NULL);
    }

    static void loop() {
        if (_delaySec < 0) return;

        if (_mfrc522.PICC_IsNewCardPresent() && _mfrc522.PICC_ReadCardSerial()) {

            String uidStr = "";
            for (byte i = 0; i < _mfrc522.uid.size; i++) {
                if (_mfrc522.uid.uidByte[i] < 0x10) {
                    uidStr += "0";
                }
                uidStr += String(_mfrc522.uid.uidByte[i], HEX);
            }

            _lastUID = uidStr;

            _lastSeen = millis();
            if (!_cardPresent && _lastSeen - _lastScanned > 2000) {
                _cardPresent = true;
                _cardStartTime = _lastSeen;
                Serial.println("card first seen");

                digitalWrite(_r, LOW);
                digitalWrite(_g, HIGH);
                digitalWrite(_b, LOW);
            }
        } else {
            if (_cardPresent && millis() - _lastSeen > 200) {
                Serial.println("card removed");
                _cardRemoved = true;
            }
        }
    }

    static void setReportCB(void (*callback)(String tag)) {
        _scannerCB = callback;
    }

    static void callBack(uint8_t arr[], size_t size) {
        char buf[size + 1];
        memcpy(buf, arr, size);
        buf[size] = '\0';
        _delaySec = atoi(buf);

        if (_delaySec >= 0) {
            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, HIGH);

            _mfrc522.PCD_Reset();
            _mfrc522.PCD_AntennaOn();

        } else {
            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, LOW);

            _mfrc522.PCD_AntennaOff();
        }
    }

private:
    static void task(void* pvParameters) {
        while (true) {
            if (_cardPresent && _delaySec >= 0) {

                unsigned long elapsed = millis() - _cardStartTime;
                
                if (elapsed >= _delaySec * 1000) {
                    if (_scannerCB) _scannerCB(_lastUID);

                    if (_delaySec == 0) vTaskDelay(pdMS_TO_TICKS(200)); // visible blink

                    digitalWrite(_r, LOW);
                    digitalWrite(_g, LOW);
                    digitalWrite(_b, HIGH);
                    
                    _lastScanned = millis(); // add delay before we can scan again

                    _cardPresent = false;
                    _cardRemoved = false;

                    _mfrc522.PICC_HaltA(); // now we call halta to stop it from scanning again
                }
                else if (_cardRemoved) {
                    digitalWrite(_r, HIGH);
                    digitalWrite(_g, LOW);
                    digitalWrite(_b, LOW);

                    vTaskDelay(pdMS_TO_TICKS(500));

                    digitalWrite(_r, LOW);
                    digitalWrite(_g, LOW);
                    digitalWrite(_b, HIGH);

                    _cardPresent = false;
                    _cardRemoved = false;
                }
            }

            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }

    inline static void (*_scannerCB)(String) = nullptr;

    volatile inline static bool _cardPresent = false;
    volatile inline static bool _cardRemoved = false;
    inline static unsigned long _cardStartTime = 0;
    inline static unsigned long _lastSeen = 0;
    inline static unsigned long _lastScanned = 0;
    inline static String _lastUID = "";
    inline static int _delaySec = -1;
    inline static int _r = -1;
    inline static int _g = -1;
    inline static int _b = -1;
    inline static int _rst = -1;
};