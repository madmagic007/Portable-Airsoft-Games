#pragma once

#include "ModuleBase.h"
#include "MFRC522.h"

class Scanner : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    void setup() override {
        _rst = _pins[4];
        _r = _pins[5];
        _g = _pins[6];
        _b = _pins[7];

        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, LOW);
        delay(50);
        digitalWrite(_rst, HIGH);
        delay(50);

        SPI.begin(_pins[1], _pins[3], _pins[2]);
        _mfrc522 = new MFRC522(_pins[1], 255); // SDA, unused RST

        _mfrc522->PCD_Init();

        pinMode(_r, OUTPUT);
        pinMode(_g, OUTPUT);
        pinMode(_b, OUTPUT);
    }

    void receiveData(uint8_t arr[], size_t size) override {
        char buf[size + 1];
        memcpy(buf, arr, size);
        buf[size] = '\0';
        _delaySec = atoi(buf);

        if (_delaySec >= 0) {
            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, HIGH);

            digitalWrite(_rst, HIGH);
            _mfrc522->PCD_Reset();
            _mfrc522->PCD_AntennaOn();

            startTask("ScannerLoopTask");
        } else {
            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, LOW);

            _mfrc522->PCD_AntennaOff();
            digitalWrite(_rst, LOW);

            stopTask();
        }
    }

    void loop() {
        if (_delaySec < 0) return;

        if (_mfrc522->PICC_IsNewCardPresent() && _mfrc522->PICC_ReadCardSerial()) {
            String uidStr = "";
            for (byte i = 0; i < _mfrc522->uid.size; i++) {
                if (_mfrc522->uid.uidByte[i] < 0x10) uidStr += "0";
                uidStr += String(_mfrc522->uid.uidByte[i], HEX);
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

private:
    void task() {
        while (true) {
            if (_cardPresent && _delaySec >= 0) {
                unsigned long elapsed = millis() - _cardStartTime;

                if (elapsed >= _delaySec * 1000) {
                    reportValue(_lastUID);

                    if (_delaySec == 0) vTaskDelay(pdMS_TO_TICKS(200)); // visible blink

                    digitalWrite(_r, LOW);
                    digitalWrite(_g, LOW);
                    digitalWrite(_b, HIGH);

                    _lastScanned = millis();
                    _cardPresent = false;
                    _cardRemoved = false;

                    _mfrc522->PICC_HaltA(); // now we call halta to stop it from scanning again
                } else if (_cardRemoved) {
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

    MFRC522* _mfrc522 = nullptr;
    int _rst = -1, _r = -1, _g = -1, _b = -1;
    int _delaySec = -1;
    bool _cardPresent = false;
    bool _cardRemoved = false;
    unsigned long _cardStartTime = 0;
    unsigned long _lastSeen = 0;
    unsigned long _lastScanned = 0;
    String _lastUID = "";
};