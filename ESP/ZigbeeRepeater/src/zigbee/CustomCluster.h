#pragma once

#include "ZigbeeEP.h"

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000

class CustomCluster : public ZigbeeEP {
public:
    CustomCluster(uint8_t endpoint, bool hasSender, bool hasReceiver);

    void sendValue(const String& str);
private:
    void defineCluster(uint16_t clusterID);
    void setValue(uint8_t arr[]);
    void reportAttr(uint16_t clusterID, uint16_t attrID);
};