package me.madmagic;

import me.madmagic.device.DeviceHandler;
import me.madmagic.gamemode.GamemodesHandler;
import me.madmagic.mqtt.MQTTHandler;
import me.madmagic.mqtt.MQTTScheduler;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.net.URISyntaxException;
import java.util.Scanner;

public class Main {

    public static File usersFile;

    public static void main(String[] args) throws Exception {
        File gamemodeFile = null;
        boolean isRegister = false;
        Scanner inScanner = new Scanner(System.in);

        if (args.length == 0) {
            while (gamemodeFile == null || !gamemodeFile.exists()) {
                System.out.print("Please input gamemode name: ");
                String line = inScanner.nextLine().toLowerCase();
                isRegister = line.equals("register");

                gamemodeFile = new File(getRelativePath("gamemodes/" + line + ".json"));
                System.out.println();

            }
        } else {
            String fileArg = args[0];
            gamemodeFile = new File(getRelativePath("gamemodes/" + fileArg + ".json"));
        }

        usersFile = new File(getRelativePath("AirsoftUsers.json"));

        if (!gamemodeFile.exists()) {
            System.out.println("Gamemode not found in gamemodes folder");
            return;
        }

        if (!usersFile.exists() && !isRegister) {
            System.out.println("AirsoftUsers.json not found. Please register users first with AirsoftRegister.jar");
        }

        JSONObject gamemode;
        try (FileInputStream fis = new FileInputStream(gamemodeFile)) {
            gamemode = new JSONObject(new JSONTokener(fis));
        } catch (JSONException e) {
            System.out.println("Failed to read gamemode json: " + e.getMessage());
            return;
        }

        JSONObject users;
        try (FileInputStream fis = new FileInputStream(usersFile)) {
            users = new JSONObject(new JSONTokener(fis));
        } catch (FileNotFoundException ignored) {
            if (isRegister) users = new JSONObject();
            else {
                System.out.println("Failed to read AirsoftUsers.json");
                return;
            }
        }

        if (users.isEmpty() && !isRegister) {
            System.out.println("AirsoftUsers.json is empty. Please register users first with AirsoftRegister.jar");
            return;
        }

        MQTTHandler.init();
        DeviceHandler.init();
        GamemodesHandler.init();
        StatsHandler.init(users);

        GamemodesHandler.startGamemode(gamemode);

        System.out.println("Program running...");
        System.out.println("Type \"stop\" to exit program safely");

        new Thread(() -> {
            while (true) {
                String line = inScanner.nextLine();

                if (GamemodesHandler.handleConsoleInput(line, inScanner)) continue;

                if (line.equals("stop")) {
                    inScanner.close();
                    GamemodesHandler.stop();
                    MQTTScheduler.stop(); // this calls MQTTHandler.stop and System.exit when the message queue is empty
                    break;
                }
            }
        }).start();
    }

    public static String getRelativePath(String relative) throws URISyntaxException {
//        Path jarDir = Paths.get(
//                        Main.class.getProtectionDomain()
//                                .getCodeSource()
//                                .getLocation()
//                                .toURI())
//                .getParent();
//
//        return jarDir.resolve(relative).normalize().toString();

        return System.getProperty("user.home") + "/Airsoft/" + relative;
    }
}