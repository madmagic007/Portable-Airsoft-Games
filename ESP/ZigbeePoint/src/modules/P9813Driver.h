#pragma once

#include "ModuleBase.h"

class P9813Driver : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    //clk, data
    void setup() override {
        pinMode(_pins[0], OUTPUT);
        pinMode(_pins[1], OUTPUT);
        
        digitalWrite(_pins[0], LOW);
        digitalWrite(_pins[1], LOW);
    }

    void callBack(uint8_t arr[], size_t size) override {
        uint8_t* taskArr;
        TaskData* data;
        
        if (size == 3) {
            taskArr = new uint8_t[3];
            for (size_t i = 0; i < 3; i++) {
                taskArr[i] = CustomCluster::hexCharToByte(arr[i]);
            }

            data = new TaskData(_pins, taskArr, size);
        } else if (size >= 6 && size % 2 == 0) {
            taskArr = new uint8_t[size / 2];
            CustomCluster::hexStringToBytes(arr, taskArr, size);
            
            data = new TaskData(_pins, taskArr, size / 2);
        } else {
            return;
        }

        xTaskCreate(
            ledTask, "ledTask", 2048,
            data, 1, NULL
        );
    }

private:
    static void ledTask(void* pvParameters) {
        TaskData* data = static_cast<TaskData*>(pvParameters);

        const uint8_t* pins = data->pins;
        uint8_t* arr = data->taskArr;

        uint8_t clk = pins[0];
        uint8_t data = pins[1];

        byte r = arr[0];
        byte g = arr[1];
        byte b = arr[2];
        
        sendByte(clk, data, 0b11000000);
        sendByte(clk, data, b);
        sendByte(clk, data, g);
        sendByte(clk, data, r);
        latch(clk, data); //submit
        
        Serial.printf("%d, %d, %d\n", r, g, b);
        
        delete data;
        vTaskDelete(NULL);
    }
    
    static void sendBit(uint8_t clk, uint8_t data, bool bitVal) {
        digitalWrite(data, bitVal);
        digitalWrite(clk, LOW);
        delayMicroseconds(1);
        digitalWrite(clk, HIGH);
        delayMicroseconds(1);
    }
    
    static void sendByte(uint8_t clk, uint8_t data, byte b) {
        for (int i = 7; i >= 0; i--) {
            sendBit(clk, data, b & (1 << i));
        }
    }

    static void latch(uint8_t clk, uint8_t data) {
        for (int i = 0; i < 36; i++) {
            sendBit(clk, data, 0);
        }
    }
};