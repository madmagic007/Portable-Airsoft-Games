package me.madmagic;

import org.json.JSONObject;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class StatsHandler {

    private static JSONObject users;
    private static JSONObject stats;
    private static File statsDir;
    private static File statsFile;

    public static void init(JSONObject users) throws Exception {
        StatsHandler.users = users;
        statsDir = new File(Main.getRelativePath("stats"));
        statsDir.mkdir();
    }

    public static void newStats(String gamemodeName) {
        LocalDateTime now = LocalDateTime.now();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd-MM-yyyy_HH:mm");

        statsFile = new File(statsDir, String.format("%s_%s.json", gamemodeName, now.format(formatter)));

        try {
            statsFile.createNewFile();
        } catch (Exception e) {
            System.out.println("failed to create statistics file: " + e.getMessage());
            System.exit(0);
        }

        stats = new JSONObject();
    }

    public static void incNumericStat(String tag, String key) {
        JSONObject tagStats = stats.optJSONObject(tag, new JSONObject());
        int curValue = tagStats.optInt(key, 0);
        tagStats.put(key, curValue + 1);
        stats.put(tag, tagStats);

        write();
    }

    public static int getNumericStat(String tag, String key) {
        JSONObject tagStats = stats.optJSONObject(tag, new JSONObject());
        return tagStats.optInt(key, 0);
    }

    public static String getName(String tag) {
        JSONObject user = users.optJSONObject(tag, new JSONObject());
        return user.optString("name", "");
    }

    public static String getTeam(String tag) {
        JSONObject user = users.optJSONObject(tag, new JSONObject());
        return user.optString("team", "");
    }

    private static void write() {
        JSONObject prettyStats = new JSONObject();

        stats.keySet().forEach(key -> {
            JSONObject user = users.optJSONObject(key, new JSONObject());
            String name = user.optString("name", key);

            prettyStats.put(name, stats.getJSONObject(key));
        });

        try (OutputStream os = new FileOutputStream(statsFile)) {
            byte[] data = prettyStats.toString(4).getBytes(StandardCharsets.UTF_8);
            os.write(data);
        } catch (Exception e) {
            System.out.println("failed to write to statistics file: " + e.getMessage());
        }
    }
}
