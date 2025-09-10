package me.madmagic.gamemode;

import me.madmagic.device.DeviceBase;
import me.madmagic.mqtt.MQTTHandler;
import org.json.JSONObject;

import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public abstract class GamemodeBase {

    protected final Map<String, DeviceBase> devices = new HashMap<>();

    @SafeVarargs
    protected final void registerDevices(Collection<DeviceBase>... withDevices) {
        devices.clear();

        for (Collection<DeviceBase> collection : withDevices) {
            for (DeviceBase device : collection) {
                devices.put(device.deviceName, device);
                device.sendModulesToMQTT();
            }
        }
    }

    public void checkReceivedData(String deviceName, String data) {
        if (!devices.containsKey(deviceName)) return;
        onTagScanned(devices.get(deviceName), data);
    }


    public void stop() {
        publishMessage("scanTagDuration", -1);
    }

    public abstract void start(JSONObject configuration);
    public abstract void onTagScanned(DeviceBase device, String value);

    public void publishMessage(String topic, Object value, String... devices) {
        if (devices == null || devices.length == 0) {
            this.devices.keySet().forEach(deviceName -> {
                MQTTHandler.publish(deviceName, topic, value);
            });
        } else {
            List<String> names = List.of(devices);

            this.devices.keySet().stream().filter(names::contains).forEach(deviceName -> {
                MQTTHandler.publish(deviceName, topic, value);
            });
        }
    }
}