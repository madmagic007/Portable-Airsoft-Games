#pragma once

#include "Arduino.h"
#include "zigbee/CustomCluster.h"

#define PIN_DATA 0
#define PIN_CLK 1

class P9813Driver {
    
    public: 
    void setup() {
        pinMode(PIN_DATA, OUTPUT);
        pinMode(PIN_CLK, OUTPUT);
        
        digitalWrite(PIN_DATA, LOW);
        digitalWrite(PIN_CLK, LOW);
    }
    
    static void callBack(uint8_t arr[], size_t size) {
        uint8_t* taskArr;
        
        if (size == 3) {
            taskArr = new uint8_t[3];
            for (size_t i = 0; i < 3; i++) {
                taskArr[i] = CustomCluster::hexCharToByte(arr[i]);
            }   
        } else if (size >= 6 && size % 2 == 0) {
            taskArr = new uint8_t[size / 2];
            CustomCluster::hexStringToBytes(arr, taskArr, size);
        } else {
            return;
        }
        
        xTaskCreate(
            ledTask, "ledTask", 2048,
            taskArr, 1, NULL
        );
    }
    
    private:
    static void ledTask(void* pvParameters) {
        uint8_t* arr = (uint8_t*)pvParameters;
        
        byte r = arr[0];
        byte g = arr[1];
        byte b = arr[2];
        
        sendByte(0b11000000);
        sendByte(b);
        sendByte(g);
        sendByte(r);
        latch(); //submit
        
        Serial.printf("%d, %d, %d\n", r, g, b);
        
        delete[] arr;
        vTaskDelete(NULL);
    }
    
    static void sendBit(bool bitVal) {
        digitalWrite(PIN_DATA, bitVal);
        digitalWrite(PIN_CLK, LOW);
        delayMicroseconds(1);
        digitalWrite(PIN_CLK, HIGH);
        delayMicroseconds(1);
    }
    
    static void sendByte(byte b) {
        for (int i = 7; i >= 0; i--) {
            sendBit(b & (1 << i));
        }
    }

    static void latch() {
        for (int i = 0; i < 36; i++) {
            sendBit(0);
        }
    }
};