package me.madmagic.gamemode;

import me.madmagic.device.*;
import org.json.JSONObject;

import java.util.Collection;

public class GamemodeBase {

    protected final DeviceCollection devices = new DeviceCollection();
    protected JSONObject configuration;

    protected final DeviceCollection spawns = new DeviceCollection();
    protected final DeviceCollection medics = new DeviceCollection();

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

    protected void parseDevices(String keyName, DeviceCollection collection, int... modules) {
        JSONObject allDevices = configuration.getJSONObject("devices");
        JSONObject allDeviceData = allDevices.optJSONObject("data", new JSONObject());

        if (!allDevices.has(keyName)) return;

        JSONObject devicesObject = allDevices.getJSONObject(keyName);
        JSONObject devices = devicesObject.getJSONObject("devices");
        JSONObject sharedData = devicesObject.optJSONObject("data", new JSONObject());

        devices.keySet().forEach(deviceName -> {
            JSONObject deviceData = devices.getJSONObject(deviceName);

            DeviceBase device = DeviceHandler.getOrCreateByName(deviceName);
            device.setModules(modules);
            device.data.clear();
            device.mergeData(allDeviceData);
            device.mergeData(sharedData);
            device.mergeData(deviceData);

            collection.put(deviceName, device);
        });
    }

    public void onTagScannedWrapper(String deviceName, String tag) {
        System.out.printf("Tag '%s' from '%s'\n", tag, deviceName);

        if (!devices.containsDevice(deviceName)) return;
        DeviceBase device = devices.get(deviceName);

        onTagScanned(device, tag);
    }

    public void start(JSONObject configuration) {
        this.configuration = configuration;

        spawns.clear();
        medics.clear();

        parseDevices("spawns", spawns, DeviceModule.GENERIC);
        parseDevices("medics", medics, DeviceModule.GENERIC);

        registerDevices(spawns.values(), medics.values());
    }

    public void stop() {
        for (DeviceBase device : devices.values()) {
            device.idle();
        }
    }

    protected void onTagScanned(DeviceBase device, String tag) {
        if (spawns.containsDevice(device)) {
            onSpawnTagScanned(device, tag);
        } else if (medics.containsDevice(device)){
            onMedicTagScanned(device, tag);
        }
    }

    protected void onSpawnTagScanned(DeviceBase device, String tag) {
        DeviceFunctions.tryRespawn(device, tag, "spawnRespawns");
    }
    protected void onMedicTagScanned(DeviceBase device, String tag) {
        DeviceFunctions.tryRespawn(device, tag, "medicRespawns");
    }
}