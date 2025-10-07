package me.madmagic.gamemode.gamemodes;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.device.DeviceHandler;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONObject;

public class Medics extends GamemodeBase {

    private final DeviceCollection medics = new DeviceCollection();

    @Override
    public void start(JSONObject configuration) {
        medics.clear();

        JSONObject devices = configuration.getJSONObject("devices");
        JSONObject medics = devices.getJSONObject("medics");
        JSONObject medicDevices = medics.getJSONObject("devices");
        JSONObject medicData = medics.optJSONObject("data");

        medicDevices.keySet().forEach(deviceName -> {
            JSONObject deviceData = medicDevices.getJSONObject(deviceName);

            DeviceBase device = DeviceHandler.getOrCreateByName(deviceName);
            device.setModules(DeviceModule.GENERIC);
            device.mergeData(medicData);
            device.mergeData(deviceData);

            this.medics.put(deviceName, device);
        });

        registerDevices(this.medics.values());
    }

    @Override
    public void stop() {

    }

    @Override
    public void onTagScanned(DeviceBase device, String value) {
        if (!medics.containsDevice(device)) return;

        String userTeam = StatsHandler.getTeam(value);

        if (!userTeam.equals(device.data.getString("team"))) {
            device.genericColorFromData("failBlink");
            return;
        }

        int curReviveCount = StatsHandler.getNumericStat(value, "revives");

        if (curReviveCount >= device.data.optInt("maxRevives")) {
            device.genericColorFromData("failBlink");
            return;
        }

        StatsHandler.incNumericStat(value, "revives");
        device.genericColorFromData("reviveBlink");
    }
}
