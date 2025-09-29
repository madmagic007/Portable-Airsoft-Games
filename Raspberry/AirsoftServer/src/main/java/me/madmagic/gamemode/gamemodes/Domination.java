package me.madmagic.gamemode.gamemodes;

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
            device.mergeData(deviceData);
            device.mergeData(capturePointDeviceData);

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
        String currentTeam = device.data.optString("capturedBy", "");
        String scannerTeam = value.equals("b4b38189") ? "red" : "blue";

        if (!currentTeam.equals(scannerTeam)) {
            if (scannerTeam.equals("red")) {
                device.setDriverColorAndBuzz(255, 0, 0, 1);
            } else {
                device.setDriverColorAndBuzz(0, 0, 255, 1);
            }

            device.data.put("capturedBy", scannerTeam);
        }
    }
}
