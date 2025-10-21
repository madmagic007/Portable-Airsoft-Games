package me.madmagic.gamemode.gamemodes;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONArray;
import org.json.JSONObject;

public class SS extends GamemodeBase {

    private final DeviceCollection points = new DeviceCollection();

    @Override
    public void start(JSONObject configuration) {
        super.start(configuration);

        parseDevices("points", points);

        registerDevices(points.values());
    }

    @Override
    protected void onTagScanned(DeviceBase device, String tag) {
        super.onTagScanned(device, tag);
        if (!points.containsDevice(device)) return;

        String deviceName = device.deviceName;
        String userTeam = StatsHandler.getUserTeam(tag);

        JSONObject user = StatsHandler.stats.optJSONObject(tag, new JSONObject());
        JSONObject team = StatsHandler.stats.optJSONObject(userTeam, new JSONObject());

        JSONArray userScanned = user.optJSONArray("scannedDevices", new JSONArray());
        JSONArray teamScanned = team.optJSONArray("scannedDevices", new JSONArray());

        // user double scanned a point
        if (StatsHandler.arrayHas(userScanned, deviceName)) {
            device.genericColorFromData("blinkFail");

            StatsHandler.incNumericStat(user, "doubleScanned");
            StatsHandler.incNumericStat(team, "totalDoubleScanned");
        } else {
            device.genericColorFromData("blink");

            //put devices in user and team array
            userScanned.put(deviceName);
            if (!StatsHandler.arrayHas(teamScanned, deviceName)) teamScanned.put(deviceName);


            // inc user / team / device totalScanned
            StatsHandler.incNumericStat(user, "devicesScanned");
            StatsHandler.incNumericStat(team, "totalScanned");
            StatsHandler.incNumericStat(deviceName, "timesScanned");
        }

        //save all stats
        user.put("scannedDevices", userScanned);
        team.put("scannedDevices", teamScanned);
        StatsHandler.stats.put(tag, user);
        StatsHandler.stats.put(userTeam, team);

        StatsHandler.write();
    }

    @Override
    public void stop() {
        super.stop();

        points.clear();
    }
}
