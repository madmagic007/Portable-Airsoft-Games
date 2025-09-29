package me.madmagic.gamemode;

import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import org.json.JSONObject;

import java.util.Collection;

public abstract class GamemodeBase {

    protected final DeviceCollection devices = new DeviceCollection();

    @SafeVarargs
    protected final void registerDevices(Collection<DeviceBase>... withDevices) {
        devices.clear();

        for (Collection<DeviceBase> collection : withDevices) {
            for (DeviceBase device : collection) {
                devices.put(device.deviceName, device);
                device.sendModulesToMQTT();
                device.applyData();
            }
        }
    }

    public void checkReceivedData(String deviceName, String data) {
        if (!devices.containsKey(deviceName)) return;
        onTagScanned(devices.get(deviceName), data);
    }


    public abstract void start(JSONObject configuration);
    public abstract void stop();
    public abstract void onTagScanned(DeviceBase device, String value);
}