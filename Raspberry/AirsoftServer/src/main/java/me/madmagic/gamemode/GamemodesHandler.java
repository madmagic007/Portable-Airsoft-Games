package me.madmagic.gamemode;

import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceHandler;
import me.madmagic.gamemode.gamemodes.Domination;
import me.madmagic.gamemode.gamemodes.Medics;
import me.madmagic.mqtt.MQTTHandler;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class GamemodesHandler {
    public static final Map<String, GamemodeBase> gamemodes = new HashMap<>() {{
        put("domination", new Domination());
        put("medics", new Medics());
    }};
    public static GamemodeBase activeGamemode;

    public static void init() {
        MQTTHandler.subscribe("airsoftPoint", (device, payload) -> {
            System.out.println("Device reported itself: " + device);

            DeviceHandler.getByName(device, dev -> {
                if (activeGamemode == null) {
                    MQTTHandler.publish(device, "data", "");
                } else {
                    dev.sendModulesToMQTT();
                    dev.applyData();
                }
            });
        });

        MQTTHandler.subscribe("scannedTag", (device, payload) -> {
            if (activeGamemode == null) return;

            String value = payload.optString("value", "");
            if (value.isBlank()) return;

            activeGamemode.checkReceivedData(device, value);
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
        gamemode.startWrapper(configuration);
        activeGamemode = gamemode;
    }

    public static void stop() {
        if (activeGamemode == null) return;

        activeGamemode.stopWrapper();
    }
}