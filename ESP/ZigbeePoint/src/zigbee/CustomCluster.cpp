#include "CustomCluster.h"

CustomCluster::CustomCluster(uint8_t endpoint, const String& senderTopic, const String& receiverKey, const String& moduleKey) : ZigbeeEP(endpoint), _senderTopic(senderTopic), _receiverKey(receiverKey), _moduleKey(moduleKey) {
    _device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID;
    _ep_config = {.endpoint = _endpoint, .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, .app_device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID, .app_device_version = 0};
    
    //cluster list
    _cluster_list = esp_zb_zcl_cluster_list_create();
    
    //mandatory clusters
    esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
}

void CustomCluster::setup(std::map<String, ModuleBase*>& modules) {
    _linkedModule = modules[_moduleKey];

    if (!_senderTopic.isEmpty()) {
        addSender(_senderTopic);

        _linkedModule->setReportCB(sendValueTrampoline, this);
    }

    if (!_receiverKey.isEmpty()) addReceiver(_receiverKey);
}

void CustomCluster::addSender(const String& topic) {
    defineCluster(SENDER_CLUSTER_ID, topic);
    _senderDefined =  true;
}

void CustomCluster::addReceiver(const String& key) {
    defineCluster(RECEIVER_CLUSTER_ID, key);
    _receiverDefined = true;
}

void CustomCluster::defineCluster(uint16_t clusterID, const String& topicKey) {
    esp_zb_attribute_list_t *cluster = esp_zb_zcl_attr_list_create(clusterID);

    uint8_t len = topicKey.length();
    uint8_t buffer[len + 1]
    buffer[0] = len;
    memcpy(buffer + 1, topicKey.c_str(), len);

    uint8_t value[] = { 0 };

    esp_zb_custom_cluster_add_custom_attr(cluster, VALUE_ATTRIBUTE_ID, ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, value);
    esp_zb_custom_cluster_add_custom_attr(cluster, TOPIC_ATTRIBUTE_ID, ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, topicKey);
    esp_zb_cluster_list_add_custom_cluster(_cluster_list, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
}

boolean CustomCluster::sendValue(uint8_t arr[]) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_status_t ret = esp_zb_zcl_set_manufacturer_attribute_val(
        _endpoint, SENDER_CLUSTER_ID, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC, VALUE_ATTRIBUTE_ID, arr, false
    );
    esp_zb_lock_release();

    if (ret != ESP_ZB_ZCL_STATUS_SUCCESS) {
        Serial.printf("Failed to set input: 0x%x: %s\n", ret, esp_zb_zcl_status_to_name(ret));
        return false;
    }

    return reportAttr(SENDER_CLUSTER_ID, VALUE_ATTRIBUTE_ID);
}

void CustomCluster::reportAttribs() {
    if (_senderDefined) {
        reportAttr(SENDER_CLUSTER_ID, TOPIC_ATTRIBUTE_ID);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (_receiverDefined) {
        reportAttr(RECEIVER_CLUSTER_ID, TOPIC_ATTRIBUTE_ID);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

boolean CustomCluster::reportAttr(uint16_t clusterID, uint16_t attrID) {
    esp_zb_zcl_report_attr_cmd_t report_attr_cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u = {
                .addr_short = 0x0000, //coordinator address, always 0x0000
            },
            .dst_endpoint = 1, //coordinator endpoint, always 1
            .src_endpoint = _endpoint,
        },
        .address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
        .clusterID = clusterID,
        .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI,
        .manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC,
        .attributeID = attrID,
    };
    
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_err_t err = esp_zb_zcl_report_attr_cmd_req(&report_attr_cmd);
    esp_zb_lock_release();
    if (err != ESP_OK) {
        Serial.printf("Failed to send report: 0x%x: %s\n", err, esp_err_to_name(err));
        return false;
    }
    return true;
}

void CustomCluster::zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) {
    uint8_t* bytes = static_cast<uint8_t*>(message->attribute.data.value);
    uint8_t length = bytes[0];
    
    uint8_t data[length];
    for (uint8_t i = 0; i < length; i++) {
        data[i] = bytes[i + 1];
    }
    
    if (_linkedModule) _linkedModule->callBack(data, length);
}