#pragma once

#include "ZigbeeEP.h"
#include "../Util.h"

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000
#define TOPIC_ATTRIBUTE_ID 0x0001

class CustomCluster : public ZigbeeEP {
public:
    CustomCluster(uint8_t endpoint, const String& senderTopic, const String& receiverKey);
    //void reportAttribs();

    bool sendValue(const String& str);
private:
    bool reportAttr(uint16_t clusterID, uint16_t attrID);

    void addSender(const String& topic);
    void addReceiver(const String& tag);
    bool setValue(uint8_t arr[]);
    void defineCluster(uint16_t clusterID, const String& topicKey);

    bool _senderDefined;
    bool _receiverDefined;
    String _senderTopic;
};