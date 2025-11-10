package me.madmagic.device;

import me.madmagic.mqtt.MQTTMessage;
import org.json.JSONObject;

public class DeviceBase {

    public final String deviceName;
    public final JSONObject data = new JSONObject();

    public DeviceBase(String deviceNam) {
        this.deviceName = deviceNam;
    }

    public void mergeData(JSONObject data) {
        data.keySet().forEach(key -> {
            this.data.put(key, data.get(key));
        });
    }

    public void applyData() {
        MQTTMessage.SCANNER.scheduleIfInData(deviceName, data);
        MQTTMessage.DRIVER_COLOR.scheduleIfInData(deviceName, data);
        MQTTMessage.GENERIC_COLOR.scheduleIfInData(deviceName, data);
    }

    public void idle() {
        data.clear();
//        setScannerSettings(-1);
//        setDriverColor(0, 0, 0);
//        setGenericColor(0, 0, 0);
//        buzz(0);
        MQTTMessage.AIRSOFTPOINT.schedule(deviceName, "restart");
    }

    public void setScannerSettings(float scanDuration) {
        MQTTMessage.SCANNER.schedule(deviceName, scanDuration);
    }

    public void setScannerSettings(float scanDuration, float buzzDuration, float buzzPause) {
        String msg = String.format("%f|%f|%f", scanDuration, buzzDuration, buzzPause);
        MQTTMessage.SCANNER.schedule(deviceName, msg);
    }

    public void buzz(int durationSeconds) {
        MQTTMessage.BUZZ.schedule(deviceName, durationSeconds);
    }

    public void setDriverColor(int r, int g, int b) {
        String hex = String.format("%02X%02X%02X", r, g, b);
        MQTTMessage.DRIVER_COLOR.schedule(deviceName, hex);
    }

    public void setDriverColorFromData(String key) {
        MQTTMessage.DRIVER_COLOR.schedule(deviceName, data.optString(key, "000000"));
    }

    public void setGenericColor(int r, int g, int b) {
        String hex = String.format("%02X%02X%02X", r, g, b);
        MQTTMessage.GENERIC_COLOR.schedule(deviceName, hex);
    }

    public void genericColorFromData(String key) {
        MQTTMessage.GENERIC_COLOR.schedule(deviceName, data.optString(key, "000000"));
    }
}