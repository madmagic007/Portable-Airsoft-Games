package me.madmagic.devices;

import me.madmagic.mqtt.MQTTHandler;

import java.util.HashMap;
import java.util.Map;

public class DeviceHandler {

    public static Map<String, Device> activeDevices = new HashMap<>();

    public static void init() {
        MQTTHandler.subscribe("airsoftPoint", (device, payload) -> {
            System.out.println("Device reported itself: " + device);
            MQTTHandler.publish(device, "data", "");
        });

        MQTTHandler.subscribe("battery", (deviceName, payload) -> {
            String data = payload.optString("data", "");
            Device device = getByName(deviceName);

            if (data.isBlank() || device == null) return;

            String[] split = data.split("\\|");
            device.batteryRawVoltage = Float.parseFloat(split[0]);
            device.batteryCorrectedVoltage = Float.parseFloat(split[1]);
            device.batteryPercentage = Float.parseFloat(split[2]);
        });
    }

    public static Device getByName(String name) {
        return activeDevices.get(name);
    }

    public static Device getOrCreateByName(String name) {
        Device device = getByName(name);

        if (device == null) {
            device = new Device(name);
            activeDevices.put(name, device);
        }

        return device;
    }
}
