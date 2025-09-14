package me.madmagic.gamemode.gamemodes;

import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceHandler;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONArray;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class Domination extends GamemodeBase {

    private final Map<String, DeviceBase> capturePoints = new HashMap<>();
    private final Map<String, DeviceBase> medics = new HashMap<>();


    @Override
    public void start(JSONObject configuration) {
        capturePoints.clear();
        medics.clear();

        JSONArray capturePointNames = configuration.getJSONObject("devices").getJSONArray("capturePoints");
        for (Object capturePoint : capturePointNames) {
            if (!(capturePoint instanceof String str)) continue;

            DeviceBase device = DeviceHandler.getOrCreateByName(str);
            device.setModules(DeviceModule.CAPTURE_POINT);
            capturePoints.put(str, device);
        }

        registerDevices(capturePoints.values(), medics.values());

        publishMessage("scannerSettings", "5|0.5|1"); // 5 second scan, 0.5s buzz, 1s pause
        publishMessage("setDriverColor", "00ff00");
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
