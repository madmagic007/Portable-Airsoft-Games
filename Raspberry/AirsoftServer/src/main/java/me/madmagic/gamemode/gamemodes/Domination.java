package me.madmagic.gamemode.gamemodes;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.device.DeviceHandler;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.GamemodeBase;
import me.madmagic.mqtt.MQTTMessage;
import org.json.JSONObject;

public class Domination extends GamemodeBase {

    private final DeviceCollection capturePoints = new DeviceCollection();
    private final DeviceCollection medics = new DeviceCollection();

    @Override
    public void start(JSONObject configuration) {
        capturePoints.clear();
        medics.clear();

        JSONObject devices = configuration.getJSONObject("devices");

        JSONObject capturePoints = devices.getJSONObject("capturePoints");
        JSONObject capturePointDevices = capturePoints.getJSONObject("devices");
        JSONObject capturePointDeviceData = capturePoints.optJSONObject("data", new JSONObject());

        capturePointDevices.keySet().forEach(deviceName -> {
            JSONObject deviceData = capturePointDevices.getJSONObject(deviceName);

            DeviceBase device = DeviceHandler.getOrCreateByName(deviceName);
            device.setModules(DeviceModule.CAPTURE_POINT);
            device.mergeData(capturePointDeviceData);
            device.mergeData(deviceData);

            this.capturePoints.put(deviceName, device);
        });

        // stores devices in GamemodeBase, registers their modules to mqtt and applies certain data
        registerDevices(this.capturePoints.values(), this.medics.values());
    }

    @Override
    public void stop() {
        devices.publishToAll(MQTTMessage.SCANNER_SETTINGS, "-1");
        capturePoints.publishToAll(MQTTMessage.DRIVER_COLOR, "000000");
        medics.publishToAll(MQTTMessage.GENERIC_COLOR, "000000");
    }

    @Override
    public void onTagScanned(DeviceBase device, String value) {
        String userTeam = StatsHandler.getTeam(value);

        if (medics.containsDevice(device)) {
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

        } else if (capturePoints.containsDevice(device)) {
            String currentTeam = device.data.optString("capturedBy", "");
            if (currentTeam.equals(userTeam)) return;

            String teamColTag = "colTeam_" + userTeam;
            device.setDriverColorFromData(teamColTag);
            device.buzz(2);

            System.out.printf("capture point %s captured by %s (%s)\n", device.deviceName, value, userTeam);
            device.data.put("capturedBy", userTeam);

        }
    }
}
