package me.madmagic.device;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

public class DeviceHandler {

    public static Map<String, DeviceBase> activeDevices = new HashMap<>();

    public static void init() {
//        MQTTHandler.subscribe("battery", (deviceName, payload) -> {
//            String value = payload.optString("value", "");
//            DeviceBase device = getByName(deviceName);
//
//            if (value.isBlank() || device == null) return;
//
//            device.data.put("battery", value);
//        });
    }

    public static DeviceBase getByName(String name) {
        return activeDevices.get(name);
    }

    public static void getByName(String name, Consumer<DeviceBase> callback) {
        DeviceBase deviceBase = getByName(name);
        if (deviceBase != null) callback.accept(deviceBase);
    }

    public static DeviceBase getOrCreateByName(String name) {
        DeviceBase device = getByName(name);

        if (device == null) {
            device = new DeviceBase(name);
            activeDevices.put(name, device);
        }

        return device;
    }
}
