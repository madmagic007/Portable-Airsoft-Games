package me.madmagic.gamemode;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceHandler;
import me.madmagic.gamemode.gamemodes.Domination;
import me.madmagic.gamemode.gamemodes.SS;
import me.madmagic.mqtt.MQTTHandler;
import me.madmagic.mqtt.MQTTMessage;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class GamemodesHandler {

    private static Map<String, Long> scanTimes = new HashMap<>();
    private static int minScansDelaySeconds = 5;

    private static final Map<String, GamemodeBase> gamemodes = new HashMap<>() {{
        put("base", new GamemodeBase());
        put("domination", new Domination());
        put("ss", new SS());
    }};
    private static GamemodeBase activeGamemode;

    public static void init() {
        MQTTHandler.subscribe("airsoftPoint", (device, payload) -> {
            //System.out.println("Device reported itself: " + device);

            DeviceHandler.getByName(device, dev -> {
                if (activeGamemode == null) {
                    MQTTMessage.DATA.schedule(device, "");
                } else {
                    dev.sendModulesToMQTT();
                    dev.applyData();
                }
            });
        });

        MQTTHandler.subscribe("scannedTag", (device, payload) -> {
            System.out.println("received handler");
            if (activeGamemode == null) return;

            String value = payload.optString("value", "");
            if (value.isBlank()) return;

            String deviceKey = device + payload;
            long curTime = System.currentTimeMillis();
            long lastScanTime = scanTimes.getOrDefault(deviceKey, 0L);

            if (curTime - lastScanTime <= minScansDelaySeconds * 1000L) return;

            scanTimes.put(deviceKey, curTime);
            activeGamemode.onTagScannedWrapper(device, value);
        });
    }

    public static void startGamemode(JSONObject configuration) {
        String gamemodeName = configuration.optString("gamemodeName", "");

        if (gamemodeName.isEmpty()) {
            System.out.println("No gamemode name is specified in the json");
            return;
        }

        GamemodeBase gamemode = gamemodes.get(gamemodeName);
        if (gamemode == null) {
            System.out.printf("Gamemode '%s' is not valid\n", gamemodeName);
            return;
        }

        System.out.println("Starting gamemode:\n" + configuration.toString(4));
        StatsHandler.newStats(gamemodeName);
        gamemode.start(configuration);
        activeGamemode = gamemode;
    }

    public static void stop() {
        if (activeGamemode == null) return;

        activeGamemode.stop();
    }
}