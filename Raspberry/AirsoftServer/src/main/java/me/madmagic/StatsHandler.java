package me.madmagic;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.function.Consumer;
import java.util.function.Function;

public class StatsHandler {

    public static JSONObject users;
    public static JSONObject stats;
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

        statsFile = new File(statsDir, java.lang.String.format("%s_%s.json", gamemodeName, now.format(formatter)));

        try {
            statsFile.createNewFile();
        } catch (Exception e) {
            System.out.println("failed to create statistics file: " + e.getMessage());
            System.exit(0);
        }

        stats = new JSONObject();
    }

    public static void getAndSaveTopLevelObject(String key, Consumer<JSONObject> consumer) {
        JSONObject o = stats.optJSONObject(key, new JSONObject());
        consumer.accept(o);
        stats.put(key, o);
        write();

    }

    public static <T> void optSave(JSONObject parent, String key, T defaultValue, Function<T, T> action) {
        Object curValue = parent.opt(key);
        curValue = curValue == null ? defaultValue : curValue;

        Object modified = action.apply((T) curValue);
        parent.put(key, modified);
    }

    public static void incNumericStat(String key, String stat) {
        getAndSaveTopLevelObject(key, o -> {
            int curValue = o.optInt(stat, 0);
            o.put(stat, curValue + 1);
        });
    }

    public static void addNumericStat(String key, String stat, int amount) {
        getAndSaveTopLevelObject(key, o -> {
            int curValue = o.optInt(stat, 0);
            o.put(stat, curValue + amount);
        });
    }

    public static void incNumericStat(JSONObject o, String stat) {
        int curValue = o.optInt(stat, 0);
        o.put(stat, curValue + 1);
    }

    public static int getNumericStat(String key, String stat) {
        JSONObject tagStats = stats.optJSONObject(key, new JSONObject());
        return tagStats.optInt(stat, 0);
    }

    public static String getUserTeam(String tag) {
        JSONObject user = users.optJSONObject(tag, new JSONObject());
        return user.optString("team", "");
    }

    public static void write() {
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

    public static boolean arrayHas(JSONArray array, Object value) {
        for (Object o : array) {
            if (o.equals(value)) return true;
        }

        return false;
    }
}
