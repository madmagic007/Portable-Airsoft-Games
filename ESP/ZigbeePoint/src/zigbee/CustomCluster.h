#pragma once

#include "ZigbeeEP.h"

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000
#define TOPIC_ATTRIBUTE_ID 0x0001

struct CustomClusterConfig {
    uint16_t endpoint;
    String senderTopic;
    String receiverKey;
};

class CustomCluster : public ZigbeeEP {
public:
    CustomCluster() = default;
    CustomCluster(const CustomClusterConfig cfg);

    void addSender(String topic);
    void addReceiver(String tag);
    void reportAttribs();

    boolean sendValue(String str);

    static uint8_t* createString(String value) {
        uint8_t len = value.length();
        uint8_t* buffer = new uint8_t[len + 1];

        buffer[0] = len;
        memcpy(buffer + 1, value.c_str(), len);

        return buffer;
    }

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

    void setReceiveCallback(void (*receiveCB)(uint8_t[], size_t)) {
        _receiveCB = receiveCB;
    }

private:
    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) override;
    boolean reportAttr(uint16_t clusterID, uint16_t attrID);

    CustomClusterConfig _cfg;
    boolean _senderDefined;
    boolean _receiverDefined;

    void (*_receiveCB)(uint8_t[], size_t);
};