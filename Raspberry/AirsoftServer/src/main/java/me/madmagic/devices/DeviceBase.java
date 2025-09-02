package me.madmagic.devices;

import java.util.Arrays;

import me.madmagic.mqtt.MQTTHandler;

public class DeviceBase {

    public final String deviceName;
    private final int[] modules;

    DeviceBase(String deviceName, int... modules) {
        this.deviceName = deviceName;
        this.modules = modules;
    }
    
    public void initializeToMqtt() {
        int max = 0;
        for (int val : modules) {
            if (val > max) max = val;
        }
        
        char[] result = new char[max + 1];
        Arrays.fill(result, '0');

        for (int val : modules) {
            result[val] = '1';
        }

        JSONObject msg = new JSONObject().put("data", new String(result));
        MQTTHandler.publish(deviceName, msg);
    }
}

public class DeviceModule {
    public static final int SCANNER = 0;
    public static final int LED_DRIVER = 1;
    public static final int BUZZER = 2;
    public static final int BATTERY_MONITOR = 3;
    public static final int GENERIC_LED = 4;
}