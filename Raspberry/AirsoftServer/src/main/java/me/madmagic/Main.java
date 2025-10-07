package me.madmagic;

import me.madmagic.device.DeviceHandler;
import me.madmagic.gamemode.GamemodesHandler;
import me.madmagic.mqtt.MQTTHandler;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.net.URISyntaxException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) throws Exception {
        String fileArg = args[0];

        File gamemodeFile = new File(getRelativePath("gamemodes/" + fileArg + ".json"));
        File usersFile = new File(getRelativePath("AirsoftUsers.json"));

        if (!gamemodeFile.exists()) {
            System.out.println("Gamemode not found in gamemodes folder");
            return;
        }

        if (!usersFile.exists()) {
            System.out.println("AirsoftUsers.json not found. Please register users first with AirsoftRegister.jar");
        }

        JSONObject gamemode;
        try (FileInputStream fis = new FileInputStream(gamemodeFile)) {
            gamemode = new JSONObject(new org.json.JSONTokener(fis));
        } catch (JSONException ignored) {
            System.out.println("Failed to read gamemode json");
            return;
        }

        JSONObject users;
        try (FileInputStream fis = new FileInputStream(usersFile)) {
            users = new JSONObject(new org.json.JSONTokener(fis));
        } catch (JSONException ignored) {
            System.out.println("Failed to read AirsoftUsers.json");
            return;
        }

        if (users.isEmpty()) {
            System.out.println("AirsoftUsers.json is empty. Please register users first with AirsoftRegister.jar");
            return;
        }

        MQTTHandler.init();
        DeviceHandler.init();
        GamemodesHandler.init();
        StatsHandler.init(users);

        GamemodesHandler.startGamemode(gamemode);

        Scanner inScanner = new Scanner(System.in);
        new Thread(() -> {
            while (true) {
                String line = inScanner.nextLine();
                if (!line.equals("stop")) continue;

                inScanner.close();
                GamemodesHandler.stop();
                MQTTHandler.stop();

                System.exit(0);
            }
        }).start();
    }

    public static String getRelativePath(String relative) throws URISyntaxException {
        Path jarDir = Paths.get(
                        Main.class.getProtectionDomain()
                                .getCodeSource()
                                .getLocation()
                                .toURI())
                .getParent();

        return jarDir.resolve(relative).normalize().toString();
    }
}