#pragma once

#include "ZigbeeController.h"
#include "ZigbeeEP.h"

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000
#define TOPIC_ATTRIBUTE_ID 0x0001

class CustomCluster : public ZigbeeEP {
public:
    CustomCluster() = default;
    CustomCluster(uint8_t endpoint, const String& senderTopic, const String& receiverKey);
    void reportAttribs();
private:
    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) override;
    bool reportAttr(uint16_t clusterID, uint16_t attrID);
    void defineCluster(uint16_t clusterID, const String& topicKey);

    String _senderTopic;
    String _receiverKey;
};