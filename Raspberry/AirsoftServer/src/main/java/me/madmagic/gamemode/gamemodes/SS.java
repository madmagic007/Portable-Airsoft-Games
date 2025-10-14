package me.madmagic.gamemode.gamemodes;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONArray;
import org.json.JSONObject;

import javax.swing.*;

public class SS extends GamemodeBase {

    private final DeviceCollection points = new DeviceCollection();

    @Override
    public void start(JSONObject configuration) {
        super.start(configuration);
        points.clear();

        parseDevices("points", points, DeviceModule.GENERIC);

        registerDevices(points.values());
    }

    @Override
    protected void onTagScanned(DeviceBase device, String tag) {
        super.onTagScanned(device, tag);
        if (!points.containsDevice(device)) return;

        String deviceName = device.deviceName;

        StatsHandler.getAndSaveTopLevelObject(tag, user -> {
            JSONArray userScanned = user.optJSONArray("scannedDevices", new JSONArray());

            if (StatsHandler.arrayHas(userScanned, deviceName)) {
                device.genericColorFromData("blinkFail");
                StatsHandler.incNumericStat(user, "doubleScanned");
                return;
            }

            userScanned.put(deviceName);
            user.put("scannedDevices", userScanned);

            StatsHandler.incNumericStat(user, "devicesScanned");
            device.genericColorFromData("blink");
        });

        String userTeam = StatsHandler.getUserTeam(tag);
        StatsHandler.getAndSaveTopLevelObject(userTeam, team -> {
            JSONArray teamScanned = team.optJSONArray("scannedDevices", new JSONArray());

            if (StatsHandler.arrayHas(teamScanned, deviceName)) {
                StatsHandler.incNumericStat(team, "totalDoubleScanned");
                return;
            }

            teamScanned.put(deviceName);
            team.put("scannedDevices", teamScanned);

            StatsHandler.incNumericStat(team, "totalScanned");
        });

        StatsHandler.incNumericStat(deviceName, "timesScanned");
    }
}
