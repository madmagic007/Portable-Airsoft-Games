package me.madmagic;

import com.diozero.devices.MFRC522;
import com.diozero.devices.RgbLed;
import com.diozero.util.Hex;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class Main {

    private static Thread thread;

    public static void main(String[] args) {
        try {
            MFRC522 scanner = new MFRC522(0, 22);
            Scanner inScanner = new Scanner(System.in);

            RgbLed led = new RgbLed(24, 23, 25);
            led.setValues(false, false, true);

            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                thread.interrupt();
                led.close();
                inScanner.close();
                scanner.close();
            }));

            String path = getRelativePath("AirsoftUsers.json");
            JSONObject tmp;

            File file = new File(path);
            file.createNewFile();

            try (FileInputStream fis = new FileInputStream(path)) {
                tmp = new JSONObject(new org.json.JSONTokener(fis));
            } catch (JSONException ignored) {
                tmp = new JSONObject();
            }
            JSONObject users = tmp;

            thread = new Thread(() -> {
                try {
                    while (!Thread.currentThread().isInterrupted()) {
                        if (!scanner.isNewCardPresent()) continue;

                        MFRC522.UID uid = scanner.readCardSerial();
                        if (uid == null) continue;

                        new Thread(() -> {
                            led.setValues(false, true, false);
                            try {
                                Thread.sleep(200);
                            } catch (InterruptedException ignored) {}
                            led.off();
                        }).start();

                        String uidStr = Hex.encodeHexString(uid.getUidBytes()).toLowerCase();
                        System.out.printf("Card scanned: %s\n", uidStr);

                        System.out.print("Choose nickname: ");
                        String nickName = inScanner.nextLine();

                        System.out.print("Choose team: ");
                        String team = inScanner.nextLine();

                        JSONObject user = users.optJSONObject(uidStr, new JSONObject());

                        if (!nickName.isBlank()) user.put("name", nickName);
                        if (!team.isBlank()) user.put("team", team);

                        users.put(uidStr, user);
                        writeJson(users, path);

                        System.out.printf("Successfully saved information for (%s): %s, %s\n", uidStr, nickName, team);

                        led.setValues(false, false, true);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });
            thread.start();

        } catch (Exception e) {
            e.printStackTrace();
        }
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

    public static void writeJson(JSONObject obj, String path) throws Exception {
        try (OutputStream os = new FileOutputStream(path)) {
            byte[] data = obj.toString(4).getBytes(StandardCharsets.UTF_8);
            os.write(data);
        }
    }
}
