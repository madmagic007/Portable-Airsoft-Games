#pragma once

#include "ZigbeeEP.h"
#include "../Util.h"
#include "../modules/ModuleBase.h"
#include <functional>
#include <map>

#define SENDER_CLUSTER_ID 0xFF00
#define RECEIVER_CLUSTER_ID 0xFF01

#define VALUE_ATTRIBUTE_ID 0x0000
#define TOPIC_ATTRIBUTE_ID 0x0001

class CustomCluster : public ZigbeeEP {
public:
    CustomCluster() = default;
    CustomCluster(uint8_t endpoint, const String& senderTopic, const String& receiverKey, const String& moduleKey);

    void setup(std::map<String, ModuleBase*>& modules);

    void addSender(const String& topic);
    void addReceiver(const String& tag);
    void reportAttribs();

    boolean sendValue(uint8_t arr[]);

    static bool sendValueTrampoline(uint8_t arr[], void* context) {
        return static_cast<CustomCluster*>(context)->sendValue(arr);
    }

private:
    void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) override;
    boolean reportAttr(uint16_t clusterID, uint16_t attrID);
    void defineCluster(uint16_t clusterID, const String& topicKey);

    boolean _senderDefined;
    boolean _receiverDefined;

    String _senderTopic;
    String _receiverKey;
    String _moduleKey;
    ModuleBase* _linkedModule;
};