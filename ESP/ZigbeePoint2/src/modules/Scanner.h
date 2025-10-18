#pragma once

#include "ModuleBase2.h"
#include "MFRC522.h"

class Scanner : public ModuleBase2 {
public:
    using ModuleBase2::ModuleBase2;

    void setup() override {
        _rst = _pins[4];
        _r = _pins[5];
        _g = _pins[6];
        _b = _pins[7];
        _buzzer = _pins[8];

        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(50);

        SPI.begin(_pins[1], _pins[3], _pins[2]);
        _mfrc522 = new MFRC522(_pins[0], 255); // SDA, unused RST

        pinOutput(_r);
        pinOutput(_g);
        pinOutput(_b);
        pinOutput(_buzzer);
    }

    void receiveData(uint8_t arr[], size_t size) override {
        char buf[size + 1];
        memcpy(buf, arr, size);
        buf[size] = '\0';
        
        uint8_t n = sscanf(buf, "%f|%f|%f", &_delaySec, &_buzzDuration, &_buzzPause);

        if (n == 1) {
            _buzzDuration = 0;
            _buzzPause = 0;
        }

        digitalWrite(_rst, LOW);

        if (_delaySec >= 0) {
            delay(50);
            digitalWrite(_rst, HIGH);
            delay(50);
            _mfrc522->PCD_Init();

            startTask("ScannerLoopTask");

            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, HIGH);
        } else {
            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, LOW);

            stopTask();
        }
    }

    void loop() {
        if (_delaySec < 0) return;

        bool present = _mfrc522->PICC_IsNewCardPresent() && _mfrc522->PICC_ReadCardSerial();
        unsigned long now = millis();

        switch (_cardState) {
            case WAIT:
                if (present) {
                    _cardDetectedTime = now;
                    _cardAbsentStart = 0;
                    _cardState = DETECTED;

                    String uidStr = "";
                    for (byte i = 0; i < _mfrc522->uid.size; i++) {
                        if (_mfrc522->uid.uidByte[i] < 0x10) uidStr += "0";
                        uidStr += String(_mfrc522->uid.uidByte[i], HEX);
                    }

                    _lastUID = uidStr;
                    _buzzState = false;

                    digitalWrite(_r, LOW);
                    digitalWrite(_g, HIGH);
                    digitalWrite(_b, LOW);
                }
                break;

            case DETECTED:
                if (!present) {
                    if (_cardAbsentStart == 0) _cardAbsentStart = now;
                    else if (now - _cardAbsentStart >= 200) {
                        _command = FAIL; 
                        _cardState = WAIT_REMOVED;
                        _cardAbsentStart = now;
                    }
                } else {
                    _cardAbsentStart = 0;
                    if (now - _cardDetectedTime >= _delaySec * 1000UL) {
                        _command = REPORT; 
                        _cardState = WAIT_REMOVED;
                    }
                }
                break;

            case WAIT_REMOVED:
                if (!present) {
                    if (_cardAbsentStart == 0) _cardAbsentStart = now;
                    else if (now - _cardAbsentStart >= 200) { // 200ms before we consider a card removed
                        _cardState = WAIT;
                        _cardAbsentStart = 0;
                    }
                } else {
                    _cardAbsentStart = 0;
                }
                break;
        }
    }

private:
    void task() {
        while (true) {
            if (_command == REPORT) {
                _command = NONE;
                sendValue(_lastUID);

                if (_delaySec == 0) vTaskDelay(pdMS_TO_TICKS(200)); // visible blink

                digitalWrite(_r, LOW);
                digitalWrite(_g, LOW);
                digitalWrite(_b, HIGH);
                digitalWrite(_buzzer, LOW);
            } else if (_command == FAIL) {
                _command = NONE;

                digitalWrite(_r, HIGH);
                digitalWrite(_g, LOW);
                digitalWrite(_b, LOW);
                digitalWrite(_buzzer, LOW);

                vTaskDelay(pdMS_TO_TICKS(500));

                digitalWrite(_r, LOW);
                digitalWrite(_g, LOW);
                digitalWrite(_b, HIGH);
            }

            if (_cardState == DETECTED && _buzzPause > 0 && _buzzDuration > 0) {
                unsigned long now = millis();

                if (_buzzState) {
                    if (now - _lastBuzz >= _buzzDuration * 1000UL) {
                        digitalWrite(_buzzer, LOW);
                        _buzzState = false;
                        _lastBuzz = now;
                    }
                } else {
                    if (now - _lastBuzz >= _buzzPause * 1000UL) {
                        digitalWrite(_buzzer, HIGH);
                        _buzzState = true;
                        _lastBuzz = now;
                    }
                }
            }

            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }

    MFRC522* _mfrc522;
    uint8_t _rst, _r, _g, _b, _buzzer;
    float _delaySec = -1;
    float _buzzDuration, _buzzPause;
    bool _buzzState;
    unsigned long _lastBuzz = 0;
    unsigned long _cardDetectedTime = 0;
    unsigned long _cardAbsentStart = 0;
    String _lastUID = "";

    enum CardState{
        WAIT,
        DETECTED,
        WAIT_REMOVED
    };

    enum TaskCommand {
        NONE,
        REPORT,
        FAIL
    };

    CardState _cardState = WAIT;
    TaskCommand _command = NONE;
};