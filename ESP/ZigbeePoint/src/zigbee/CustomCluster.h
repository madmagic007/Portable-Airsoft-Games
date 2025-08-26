#pragma once

#include "ZigbeeEP.h"
#include <functional>

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

    boolean sendValue(uint8_t arr[]);

    void setReceiveCB(std::function<void(uint8_t[], size_t)> receiveCB) {
        _receiveCB = receiveCB;
    }

private:
    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) override;
    boolean reportAttr(uint16_t clusterID, uint16_t attrID);

    CustomClusterConfig _cfg;
    boolean _senderDefined;
    boolean _receiverDefined;

    std::function<void(uint8_t[], size_t)> _receiveCB;
};