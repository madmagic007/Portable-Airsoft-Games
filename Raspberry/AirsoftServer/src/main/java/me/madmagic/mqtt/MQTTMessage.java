package me.madmagic.mqtt;

import org.json.JSONObject;

public enum MQTTMessage {
    SCANNER_SETTINGS("scannerSettings"),
    DRIVER_COLOR("setDriverColor"),
    BUZZ("buzz"),
    GENERIC_COLOR("setGenericColor");

    private final String topic;

    MQTTMessage(String topic) {
        this.topic = topic;
    }

    public void publish(String deviceName, Object value) {
        MQTTHandler.publish(deviceName, topic, value);
    }

    public void publishIfInData(String deviceName, JSONObject data) {
        if (data.has(topic)) publish(deviceName, data.get(topic));
    }
}
