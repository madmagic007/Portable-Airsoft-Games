package me.madmagic.gamemode.gamemodes;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONObject;

public class Domination extends GamemodeBase {

    private final DeviceCollection capturePoints = new DeviceCollection();

    @Override
    public void start(JSONObject configuration) {
        capturePoints.clear();

        parseDevices("capturePoints", capturePoints, DeviceModule.CAPTURE_POINT);

        registerDevices(capturePoints.values());
    }

    @Override
    public void stop() {}

    @Override
    public void onTagScanned(DeviceBase device, String tag) {
       if (!capturePoints.containsDevice(device)) return;

        String userTeam = StatsHandler.getTeam(tag);
        String currentTeam = device.data.optString("capturedBy", "");
        if (currentTeam.equals(userTeam)) return;

        String teamColTag = "colTeam_" + userTeam;
        device.setDriverColorFromData(teamColTag);
        device.buzz(2);

        System.out.printf("capture point %s captured by %s (%s)\n", device.deviceName, tag, userTeam);
        device.data.put("capturedBy", userTeam);
    }
}
