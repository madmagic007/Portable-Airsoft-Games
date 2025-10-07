package me.madmagic.device;

import me.madmagic.mqtt.MQTTHandler;
import me.madmagic.mqtt.MQTTMessage;
import org.json.JSONObject;

import java.util.Arrays;

public class DeviceBase {

    public final String deviceName;
    public int[] modules;
    public final JSONObject data = new JSONObject();

    public DeviceBase(String deviceNam) {
        this.deviceName = deviceNam;
    }

    public void setModules(int... modules) {
        this.modules = modules;
    }

    public void mergeData(JSONObject data) {
        data.keySet().forEach(key -> {
            this.data.put(key, data.get(key));
        });
    }

    public void sendModulesToMQTT() {
        if (modules == null || modules.length == 0) {
            MQTTHandler.publish(deviceName, "data", "");
            return;
        }

        int max = 0;
        for (int val : modules) {
            if (val > max) max = val;
        }

        char[] result = new char[max + 1];
        Arrays.fill(result, '0');

        for (int val : modules) {
            result[val] = '1';
        }

        MQTTHandler.publish(deviceName, "data", new String(result));
    }

    public void applyData() {
        MQTTMessage.SCANNER_SETTINGS.publishIfInData(deviceName, data);
        MQTTMessage.DRIVER_COLOR.publishIfInData(deviceName, data);
        MQTTMessage.GENERIC_COLOR.publishIfInData(deviceName, data);
    }

    public void idle() {
        data.clear();
        setScannerSettings(-1);
        setDriverColor(0, 0, 0);
        setGenericColor(0, 0, 0);
        buzz(0);
    }

    public void setScannerSettings(float scanDuration) {
        MQTTMessage.SCANNER_SETTINGS.publish(deviceName, scanDuration);
    }

    public void setScannerSettings(float scanDuration, float buzzDuration, float buzzPause) {
        String msg = String.format("%f|%f|%f", scanDuration, buzzDuration, buzzPause);
        MQTTMessage.SCANNER_SETTINGS.publish(deviceName, msg);
    }

    public void buzz(int durationSeconds) {
        MQTTMessage.BUZZ.publish(deviceName, durationSeconds);
    }

    public void setDriverColor(int r, int g, int b) {
        String hex = String.format("%02X%02X%02X", r, g, b);
        MQTTMessage.DRIVER_COLOR.publish(deviceName, hex);
    }

    public void setDriverColorFromData(String key) {
        MQTTMessage.DRIVER_COLOR.publish(deviceName, data.optString(key, "000000"));
    }

    public void setGenericColor(int r, int g, int b) {
        String hex = String.format("%02X%02X%02X", r, g, b);
        MQTTMessage.GENERIC_COLOR.publish(deviceName, hex);
    }

    public void genericColorFromData(String key) {
        MQTTMessage.GENERIC_COLOR.publish(deviceName, data.optString(key, "000000"));
    }
}