package me.madmagic.mqtt;

import org.json.JSONObject;

public enum MQTTMessage {
    SCANNER_SETTINGS("scannerSettings"),
    DRIVER_COLOR("setDriverColor"),
    BUZZ("buzz"),
    GENERIC_COLOR("setGenericColor"),
    DATA("data");

    private final String topic;

    MQTTMessage(String topic) {
        this.topic = topic;
    }

    public void schedule(String deviceName, Object value) {
        MQTTScheduler.schedule(deviceName, topic, value.toString());
    }

    public void scheduleIfInData(String deviceName, JSONObject data) {
        if (data.has(topic)) schedule(deviceName, data.get(topic));
    }
}
