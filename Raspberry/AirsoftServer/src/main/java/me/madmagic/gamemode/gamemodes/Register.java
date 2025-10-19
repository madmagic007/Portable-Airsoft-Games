package me.madmagic.gamemode.gamemodes;

import me.madmagic.Main;
import me.madmagic.StatsHandler;
import me.madmagic.device.DeviceBase;
import me.madmagic.device.DeviceCollection;
import me.madmagic.device.DeviceModule;
import me.madmagic.gamemode.GamemodeBase;
import org.json.JSONObject;

import java.io.FileOutputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class Register extends GamemodeBase {

    private final DeviceCollection scanner = new DeviceCollection();

    private String deviceName = "";
    private String lastTag = "";
    private String name = "";
    private String team = "";
    private int step = 0;

    @Override
    public void start(JSONObject configuration) {
        super.start(configuration);
        parseDevices("scanner", scanner, DeviceModule.GENERIC);
        registerDevices(scanner.values());
    }


    @Override
    protected void onTagScanned(DeviceBase device, String tag) {
        device.setScannerSettings(-1);
        System.out.printf("Card scanned: %s\n", tag);
        System.out.print("Choose nickname: ");
        step = 1;
        lastTag = tag;
        deviceName = device.deviceName;
    }

    @Override
    protected boolean onConsoleInput(String input, Scanner inScanner) {
        if (step == 0) return false;
        else if (step == 1) {
            name = input;
            System.out.print("Choose team: ");
            step = 2;
        } else if (step == 2) {
            team = input;

            JSONObject users = StatsHandler.users;

            StatsHandler.optSave(users, lastTag, new JSONObject(), user -> {
                if (!name.isBlank()) user.put("name", name);
                if (!team.isBlank()) user.put("team", team);

                System.out.printf("Successfully saved information for (%s): %s, %s\n", lastTag, name, team);

                scanner.get(deviceName).setScannerSettings(0);

                return user;
            });

            try (OutputStream os = new FileOutputStream(Main.usersFile)) {
                byte[] data = users.toString(4).getBytes(StandardCharsets.UTF_8);
                os.write(data);
            } catch (Exception e) {
                System.out.println("failed to write to users file: " + e.getMessage());
            }

            step = 0;
        }

        return true;
    }
}
