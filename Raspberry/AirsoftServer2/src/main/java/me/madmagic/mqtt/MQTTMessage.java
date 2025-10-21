package me.madmagic.mqtt;

import org.json.JSONObject;

public enum MQTTMessage {
    AIRSOFTPOINT(1, ""),
    SCANNER(2, "scannerSettings"),
    DRIVER_COLOR(3, "driverColor"),
    BUZZ(4, ""),
    GENERIC_COLOR(5, "genericColor");

    public final int endpoint;
    private final String configKey;

    MQTTMessage(int endpoint, String configKey) {
        this.endpoint = endpoint;
        this.configKey = configKey;
    }

    public void schedule(String deviceName, Object value) {
        MQTTScheduler.schedule(deviceName, endpoint, value.toString());
    }

    public void scheduleIfInData(String deviceName, JSONObject data) {
        if (!configKey.isEmpty() && data.has(configKey)) schedule(deviceName, data.get(configKey));
    }
}
