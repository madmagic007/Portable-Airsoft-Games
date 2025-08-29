#pragma once

class Util {
public:
    static void hexStringToBytes(const uint8_t* hexStr, uint8_t* byteArr, size_t byteArrSize) {
        for (size_t i = 0; i < byteArrSize; i++) {
            char buf[3] = { hexStr[i*2], hexStr[i*2 + 1], '\0' }; // take 2 chars
            byteArr[i] = (uint8_t) strtol(buf, NULL, 16);          // convert to byte
        }
    }

    static uint8_t hexCharToByte(uint8_t c) {
        uint8_t val = (c <= '9') ? (c - '0') : (c - 'A' + 10); // 0-15
        return (val * 255) / 15;
    }
};