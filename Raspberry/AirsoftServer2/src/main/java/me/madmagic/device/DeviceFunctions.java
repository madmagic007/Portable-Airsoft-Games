package me.madmagic.device;

import me.madmagic.StatsHandler;

public class DeviceFunctions {

    public static void tryRespawn(DeviceBase device, String tag, String statName) {
        String userTeam = StatsHandler.getUserTeam(tag);

        if (!userTeam.equals(device.data.getString("team"))) {
            device.genericColorFromData("failBlink");
            return;
        }

        int maxRespawns = device.data.optInt("maxRespawns", -1);
        int statRespawns = StatsHandler.getNumericStat(tag, statName);

        if (maxRespawns >= 0 && statRespawns >= maxRespawns) {
            device.genericColorFromData("failBlink");
            StatsHandler.incNumericStat(tag, "failedRevives");
            return;
        }

        StatsHandler.incNumericStat(tag, "totalRespawns");
        StatsHandler.incNumericStat(tag, statName);

        device.genericColorFromData("reviveBlink");
    }
}
