package me.madmagic.gamemodes;

import me.madmagic.devices.Device;
import me.madmagic.mqtt.MQTTHandler;

import java.util.HashMap;
import java.util.Map;

public abstract class GamemodeBase {

    private Map<String, Device> devices = new HashMap<>();

    public void startBase(Device... withDevices) {
        for (Device device : withDevices) {
            devices.put(device.deviceName, device);
        }

        MQTTHandler.subscribe("scannedTag", (device, payload) -> {
            if (!devices.containsKey(device)) return;

            String data = payload.optString("data", "");
            if (data.isBlank()) return;

            onTagScanned(device, data);
        });
    }

    public abstract void start();
    public abstract void onTagScanned(String device, String value);
}