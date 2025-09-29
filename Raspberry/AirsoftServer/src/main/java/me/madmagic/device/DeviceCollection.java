package me.madmagic.device;

import me.madmagic.mqtt.MQTTMessage;

import java.util.HashMap;

public class DeviceCollection extends HashMap<String, DeviceBase> {

    public void publishToAll(MQTTMessage message, Object value) {
        keySet().forEach(deviceName -> message.publish(deviceName, value));
    }
}
