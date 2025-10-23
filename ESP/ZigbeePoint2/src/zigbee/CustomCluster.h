#pragma once

#include "ZigbeeEP.h"
#include "../Util.h"

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000

class CustomCluster : public ZigbeeEP {
public:
    CustomCluster(uint8_t endpoint);

    bool sendValue(const String& str);
private:
    void defineCluster(uint16_t clusterID);
    bool setValue(uint8_t arr[]);
    bool reportValue();
};