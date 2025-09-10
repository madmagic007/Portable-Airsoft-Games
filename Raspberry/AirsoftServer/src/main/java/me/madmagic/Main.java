package me.madmagic;

import me.madmagic.device.DeviceHandler;
import me.madmagic.gamemode.GamemodesHandler;
import me.madmagic.mqtt.MQTTHandler;
import org.json.JSONArray;
import org.json.JSONObject;

public class Main {

    public static void main(String[] args) throws Exception {
        MQTTHandler.init();
        DeviceHandler.init();
        GamemodesHandler.init();

        GamemodesHandler.startGamemode(
                new JSONObject()
                        .put("gamemodeName", "domination")
                        .put("devices", new JSONObject()
                                .put("capturePoints", new JSONArray().put("test"))
                        )
        );
    }
}