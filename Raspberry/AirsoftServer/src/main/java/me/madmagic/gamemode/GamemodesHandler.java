package me.madmagic.gamemode;

import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceHandler;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.gamemodes.Domination;
import me.madmagic.mqtt.MQTTHandler;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class GamemodesHandler {
    public static final Map<String, GamemodeBase> gamemodes = new HashMap<>() {{
        put("domination", new Domination());
    }};
    public static GamemodeBase activeGamemode;

    public static void init() {
        MQTTHandler.subscribe("airsoftPoint", (device, payload) -> {
            System.out.println("Device reported itself: " + device);

            try {
                DeviceBase d = DeviceHandler.getOrCreateByName(device);
                d.setModules(DeviceModule.GENERIC);
                d.sendModulesToMQTT();
                MQTTHandler.publish(device, "scannerSettings", "0");
                MQTTHandler.publish(device, "setGenericColor", "0071fd");
            } catch (Exception ignored){}

            //TODO scanner settings need to be reported
//            if (activeGamemode == null) {
//                MQTTHandler.publish(device, "data", "");
//            } else {
//                DeviceHandler.getByName(device, DeviceBase::sendModulesToMQTT);
//            }
        });

        MQTTHandler.subscribe("scannedTag", (device, payload) -> {
            if (activeGamemode == null) return;

            String value = payload.optString("value", "");
            if (value.isBlank()) return;

            activeGamemode.checkReceivedData(device, value);
        });
    }

    public static void startGamemode(JSONObject configuration) {
        String gamemodeName = configuration.getString("gamemodeName");

        GamemodeBase gamemode = gamemodes.get(gamemodeName);
        if (gamemode == null) return;

        System.out.println("Starting gamemode: " + configuration.toString(4));
        gamemode.start(configuration);
        activeGamemode = gamemode;
    }
}