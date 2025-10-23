#include "CustomCluster.h"
#include "../modules/AirsoftPoint.h"

CustomCluster::CustomCluster(uint8_t endpoint) : ZigbeeEP(endpoint) {
    _device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID;
    _ep_config = {.endpoint = _endpoint, .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, .app_device_id = ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID, .app_device_version = 0};
    
    //cluster list
    _cluster_list = esp_zb_zcl_cluster_list_create();
    
    //mandatory clusters
    esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    defineCluster(SENDER_CLUSTER_ID);
    defineCluster(RECEIVER_CLUSTER_ID);
}

void CustomCluster::defineCluster(uint16_t clusterID) {
    esp_zb_attribute_list_t *cluster = esp_zb_zcl_attr_list_create(clusterID);
    uint8_t value[] = { 0 };

    esp_zb_custom_cluster_add_custom_attr(cluster, VALUE_ATTRIBUTE_ID, ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, value);
    esp_zb_cluster_list_add_custom_cluster(_cluster_list, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
}

bool CustomCluster::setValue(uint8_t arr[]) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_status_t ret = esp_zb_zcl_set_manufacturer_attribute_val(
        _endpoint, SENDER_CLUSTER_ID, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC, VALUE_ATTRIBUTE_ID, arr, false
    );
    esp_zb_lock_release();

    if (ret != ESP_ZB_ZCL_STATUS_SUCCESS) {
        Serial.printf("Failed to set input: 0x%x: %s\n", ret, esp_zb_zcl_status_to_name(ret));
        return false;
    }
    
    return true;
}

bool CustomCluster::sendValue(const String& str) {
    String combined = String(_endpoint) + "|" + str;
    uint8_t len = combined.length();
    uint8_t buffer[len + 1];
    buffer[0] = len;
    memcpy(buffer + 1, combined.c_str(), len);

    setValue(buffer);
    return reportValue();
}

boolean CustomCluster::reportValue() {
    esp_zb_zcl_report_attr_cmd_t report_attr_cmd = {
        .zcl_basic_cmd = {
            .dst_addr_u = {
                .addr_short = 0x0000, //coordinator address, always 0x0000
            },
            .dst_endpoint = 1, //coordinator endpoint, always 1
            .src_endpoint = _endpoint,
        },
        .address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
        .clusterID = SENDER_CLUSTER_ID,
        .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI,
        .manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC,
        .attributeID = VALUE_ATTRIBUTE_ID,
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
    AirsoftPoint::confirmed();
}