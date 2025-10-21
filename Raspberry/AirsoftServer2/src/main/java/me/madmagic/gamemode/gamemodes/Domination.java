package me.madmagic.gamemode.gamemodes;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONObject;

public class Domination extends GamemodeBase {

    private final DeviceCollection hardPoints = new DeviceCollection();
    private final DeviceCollection softPoints = new DeviceCollection();

    @Override
    public void start(JSONObject configuration) {
        super.start(configuration);

        parseDevices("hardPoints", hardPoints);
        parseDevices("softPoints", softPoints);

        registerDevices(hardPoints.values(), softPoints.values());
    }

    @Override
    public void onTagScanned(DeviceBase device, String tag) {
        super.onTagScanned(device, tag);
        String userTeam = StatsHandler.getUserTeam(tag);
        String capturePointTeam = device.data.optString("capturedBy", "");

        if (capturePointTeam.equals(userTeam)) return;

        String teamColKey = "colTeam_" + userTeam;

        if (hardPoints.containsDevice(device)) {
            device.setDriverColorFromData(teamColKey);
            device.buzz(2);
        } else if (softPoints.containsDevice(device)) {
            device.genericColorFromData(teamColKey);
        } else {
            return;
        }

        long curTime = System.currentTimeMillis();
        long since = device.data.optLong("capturedSince", 0);

        if (!capturePointTeam.isEmpty() && since != 0) {
            int capturedDuration = Math.round((curTime - since) / 1000F);

            StatsHandler.addNumericStat(capturePointTeam, "totalCaptureTime", capturedDuration);
        }

        System.out.printf("capture point %s captured by %s (%s)\n", device.deviceName, tag, userTeam);
        device.data.put("capturedBy", userTeam);
        device.data.put("capturedSince", curTime);
        StatsHandler.incNumericStat(tag, "pointsCaptured");
    }

    @Override
    public void stop() {
        long curTime = System.currentTimeMillis();

        devices.values().forEach(device -> {
            System.out.println(device.data.toString(4));
            long since = device.data.optLong("capturedSince", 0);
            String capturePointTeam = device.data.optString("capturedBy", "");

            if (!capturePointTeam.isEmpty() && since != 0) {
                int capturedDuration = Math.round((curTime - since) / 1000F);

                StatsHandler.addNumericStat(capturePointTeam, "totalCaptureTime", capturedDuration);
            }
        });

        super.stop();

        hardPoints.clear();
        softPoints.clear();
    }
}
