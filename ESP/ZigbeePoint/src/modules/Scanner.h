#pragma once

#include "ModuleBase2.h"
#include "PN532_HSU.h"
#include "PN532.h"

static PN532_HSU pn532hsu(Serial1, RX, TX);
static PN532 nfc(pn532hsu);

class Scanner : public ModuleBase2 {
    public:
    using ModuleBase2::ModuleBase2;
    
    void setup() override {
        _r = _pins[0];
        _g = _pins[1];
        _b = _pins[2];
        _buzzer = _pins[3];

        nfc.begin();
        nfc.SAMConfig();
        delay(50);
        nfc.powerDownMode();
        
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
        
        if (_delaySec >= 0) {
            pn532hsu.wakeup();
            startTask("ScannerLoopTask");
            
            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, HIGH);
        } else {
            nfc.powerDownMode();
            stopTask();

            digitalWrite(_r, LOW);
            digitalWrite(_g, LOW);
            digitalWrite(_b, LOW);
        }
    }
    
    void loop() {
        if (_delaySec < 0) return;
        
        uint8_t success;
        uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
        uint8_t uidLength;
        
        bool present = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);
        unsigned long now = millis();
        
        switch (_cardState) {
            case WAIT:
                if (present) {
                    _lastUID = getUidStr(uid, uidLength);
                    _cardDetectedTime = now;
                    _cardAbsentStart = 0;
                    _cardState = DETECTED;
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
                
                if (_delaySec == 0) delay(200); // visible blink
                
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
                
                delay(500);
                
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
            
            delay(50);
        }
    }
    
    String getUidStr(const uint8_t *buffer, size_t length) {
        String result = "";
        for (size_t i = 0; i < length; i++) {
            if (buffer[i] < 0x10) result += "0";
            result += String(buffer[i], HEX);
        }
        return result;
    }
    
    uint8_t _r, _g, _b, _buzzer;
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