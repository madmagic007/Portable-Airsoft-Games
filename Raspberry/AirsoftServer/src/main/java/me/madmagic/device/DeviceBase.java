package me.madmagic.device;

import me.madmagic.mqtt.MQTTHandler;
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

    public void setDriverColor(int r, int g, int b) {
        String hex = String.format("%02X%02X%02X", r, g, b);
        MQTTHandler.publish(deviceName, "setDriverColor", hex);
    }

    public void buzz(int durationSeconds) {
        MQTTHandler.publish(deviceName, "buzz", durationSeconds);
    }

    public void setDriverColorAndBuzz(int r, int g, int b, int durationSeconds) {
        setDriverColor(r, g, b);
        buzz(durationSeconds);
    }

    public void setGenericColor(int r, int g, int b) {
        String hex = String.format("%02X%02X%02X", r, g, b);
        MQTTHandler.publish(deviceName, "setGenericColor", hex);
    }

    public void setScannerSettings(float scanDuration, float buzzDuration, float buzzPause) {
        String msg = String.format("%f|%f|%f", scanDuration, buzzDuration, buzzPause);
        MQTTHandler.publish(deviceName, "scannerSettings", msg);
    }
}