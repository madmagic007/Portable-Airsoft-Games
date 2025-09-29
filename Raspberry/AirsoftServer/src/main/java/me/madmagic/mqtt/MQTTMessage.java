package me.madmagic.mqtt;

public enum MQTTMessages {
    SCANNER_SETTINGS("scannerSettings"),
    DRIVER_COLOR("setDriverColor"),
    BUZZ("buzz"),
    GENERIC_COLOR("setGenericColor");

    private final String topic;

    MQTTMessages(String topic) {
        this.topic = topic;
    }

    public void publish(String deviceName, Object value) {
        MQTTHandler.publish(deviceName, topic, value);
    }
}
