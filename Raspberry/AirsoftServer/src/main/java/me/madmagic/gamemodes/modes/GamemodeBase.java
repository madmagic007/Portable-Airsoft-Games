package me.madmagic.gamemodes.modes;

import java.util.Map;

import me.madmagic.devices.DeviceBase;

public abstract class GamemodeBase {
    
    private final String[] mqttTopics;
    private Map<String, DeviceBase> devices = new HashMap<>();
    
    public GamemodeBase(String... mqttTopics) {
        this.mqttTopics = mqttTopics;
    }
    
    public void start(DeviceBase... withDevices) {
        for (DeviceBase device : withDevices) {
            devices.put(device.deviceName , device);
        }
        
        for (String topic : mqttTopics) {
            MQTTHandler.subscribe(topic, (device, payload) -> {
                if (!withDevices.contains(device)) return;
                topicReceived(topic, device, payload.toString());
            });
        }
    }
    
    abstract void topicReceived(String topic, String device, Stirng value);
}