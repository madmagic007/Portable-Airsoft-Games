package me.madmagic.device;

import me.madmagic.mqtt.MQTTHandler;

import java.util.Arrays;

public class Device {

    public final String deviceName;
    public int[] modules;

    public float batteryRawVoltage;
    public float batteryCorrectedVoltage;
    public float batteryPercentage;

    public Device(String deviceNam) {
        this.deviceName = deviceNam;
    }

    public void initializeToMqtt(int... modules) {
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
}